#include "DMA.hh"

#include <cmath>

#include "AXI4Bus.hh"
#include "MemPacket.hh"
#include "event/AXI4BusAcqEvent.hh"
#include "event/DMAEndEvent.hh"
#include "event/DMAReqDoneEvent.hh"
#include "event/DMAStartEvent.hh"

DMA::DMA(std::string _name, int _base_addr, int _size)
    : acalsim::SimModule(_name), base_addr(_base_addr), size(_size) {}

void DMA::dmaReqHandler(acalsim::Tick _when, MemReqPacket* _memReqPkt) {
	this->pending_req_queue.push(_memReqPkt);
	if (this->is_idle) {
		this->is_idle = false;
		triggerNextReq();
	}
}

void DMA::triggerNextReq() {
	// check bit set
	if (!this->pending_req_queue.empty()) {
		auto pkt = this->pending_req_queue.front();
		if (auto readPkt = dynamic_cast<MemReadReqPacket*>(pkt)) {
			dmaReadReqHandler(readPkt);
		} else if (auto writePkt = dynamic_cast<MemWriteReqPacket*>(pkt)) {
			dmaWriteReqHandler(writePkt);
		}
		this->pending_req_queue.pop();
	} else {
		this->is_idle = true;
	}
}

void DMA::dmaReadReqHandler(MemReadReqPacket* _memReadReqPkt) {
	instr               i      = _memReadReqPkt->getInstr();
	instr_type          op     = _memReadReqPkt->getOP();
	uint32_t            addr   = _memReadReqPkt->getAddr();
	operand             a1     = _memReadReqPkt->getA1();
	acalsim::SimModule* sender = _memReadReqPkt->getSender();

	size_t   bytes = 0;
	uint32_t ret   = 0;

	switch (op) {
		case LB:
		case LBU: bytes = 1; break;
		case LH:
		case LHU: bytes = 2; break;
		case LW: bytes = 4; break;
	}

	ASSERT_MSG(addr + bytes <= this->base_addr + this->size, "The DMA memory region is out of range.");
	void* data = (uint8_t*)this->rf + (addr - this->base_addr);

	switch (op) {
		case LB: ret = static_cast<uint32_t>(*(int8_t*)data); break;
		case LBU: ret = *(uint8_t*)data; break;
		case LH: ret = static_cast<uint32_t>(*(int16_t*)data); break;
		case LHU: ret = *(uint16_t*)data; break;
		case LW: ret = *(uint32_t*)data; break;
	}

	auto               rc         = acalsim::top->getRecycleContainer();
	MemReadRespPacket* memRespPkt = rc->acquire<MemReadRespPacket>(&MemReadRespPacket::renew, i, op, ret, a1, sender);
	AXI4BusAcqEvent*   axi4BusAcqEvent = rc->acquire<AXI4BusAcqEvent>(
        &AXI4BusAcqEvent::renew, dynamic_cast<AXI4Bus*>(this->getDownStream("DSAXI4Bus")), memRespPkt);
	DMAReqDoneEvent* dmaReqDoneEvent = rc->acquire<DMAReqDoneEvent>(&DMAReqDoneEvent::renew, this);
	this->scheduleEvent(axi4BusAcqEvent, acalsim::top->getGlobalTick() + 2);
	this->scheduleEvent(dmaReqDoneEvent, acalsim::top->getGlobalTick() + 1);
	rc->recycle(_memReadReqPkt);
}

void DMA::dmaWriteReqHandler(MemWriteReqPacket* _memWriteReqPkt) {
	instr_type op   = _memWriteReqPkt->getOP();
	uint32_t   addr = _memWriteReqPkt->getAddr();
	uint32_t   data = _memWriteReqPkt->getData();
	size_t     bytes;

	switch (op) {
		case SB: bytes = 1; break;
		case SH: bytes = 2; break;
		case SW: bytes = 4; break;
	}

	ASSERT_MSG(addr + bytes < this->base_addr + this->size, "The DMA memory region is out of range.");

	switch (op) {
		case SB: {
			uint8_t val8 = static_cast<uint8_t>(data);
			std::memcpy((uint8_t*)this->rf + addr - this->base_addr, &val8, sizeof(uint8_t));
			break;
		}
		case SH: {
			uint16_t val16 = static_cast<uint16_t>(data);
			std::memcpy((uint8_t*)this->rf + addr - this->base_addr, &val16, sizeof(uint16_t));
			break;
		}
		case SW: {
			uint32_t val32 = static_cast<uint32_t>(data);
			std::memcpy((uint8_t*)this->rf + addr - this->base_addr, &val32, sizeof(uint32_t));
			break;
		}
	}

	auto             rc              = acalsim::top->getRecycleContainer();
	DMAReqDoneEvent* dmaReqDoneEvent = rc->acquire<DMAReqDoneEvent>(&DMAReqDoneEvent::renew, this);
	this->scheduleEvent(dmaReqDoneEvent, acalsim::top->getGlobalTick() + 1);
	if (this->rf[0] & 1) {
		DMAStartEvent* dmaStartEvent = rc->acquire<DMAStartEvent>(&DMAStartEvent::renew, this);
		this->scheduleEvent(dmaStartEvent, acalsim::top->getGlobalTick() + 1);
	}
	rc->recycle(_memWriteReqPkt);
}

void DMA::dmaStart() {
	auto srcBaseAddr  = rf[1];
	auto destBaseAddr = rf[2];
	auto tensorHeight = rf[3] & 0xff;
	auto tensorWidth  = (rf[3] >> 8) & 0xff;
	auto destStride   = (rf[3] >> 16) & 0xff;
	auto srcStride    = (rf[3] >> 24) & 0xff;

	if (tensorHeight > 1) {
		ASSERT_MSG(srcStride >= tensorWidth, "Wrong DMA config.");
		ASSERT_MSG(destStride >= tensorWidth, "Wrong DMA config.");
	}

	auto    rc = acalsim::top->getRecycleContainer();
	int     readBytes;
	instr   ins;
	operand op;

	for (int h = 0; h < tensorHeight; h++) {
		readBytes = 0;
		while (tensorWidth - readBytes) {
			uint32_t addr      = srcBaseAddr + h * srcStride + readBytes;
			int      burstSize = 1;

			if (static_cast<AXI4Bus*>(this->getDownStream("DSAXI4Bus"))->getBurstMode() == BurstMode::INCR) {
				if (tensorWidth - readBytes <= 4) {
					burstSize = 1;
				} else if (tensorWidth - readBytes <= 8) {
					burstSize = 2;
				} else {
					burstSize = 4;
				}
			}

			MemReadReqPacket* memReadReqPkt =
			    rc->acquire<MemReadReqPacket>(&MemReadReqPacket::renew, ins, LW, addr, op, burstSize, this);
			int reqBytes = tensorWidth - readBytes >= 4 * burstSize ? 4 * burstSize : tensorWidth - readBytes;
			this->dma_info_queue.push({memReadReqPkt, reqBytes, burstSize, destBaseAddr + h * destStride + readBytes});
			readBytes += reqBytes;
		}
	}

	triggerNextDMA(0);
}

void DMA::triggerNextDMA(int _latency) {
	auto rc = acalsim::top->getRecycleContainer();

	if (!this->dma_info_queue.empty()) {
		auto dmaInfo        = this->dma_info_queue.front();
		this->curBurstCount = 0;
		this->curBurstSize  = dmaInfo.burstSize;
		this->curDestAddr   = dmaInfo.destAddr;
		this->curBytes      = dmaInfo.bytes;

		AXI4BusAcqEvent* event = rc->acquire<AXI4BusAcqEvent>(
		    &AXI4BusAcqEvent::renew, dynamic_cast<AXI4Bus*>(this->getDownStream("DSAXI4Bus")), dmaInfo.memReadReqPkt);
		this->scheduleEvent(event, acalsim::top->getGlobalTick() + 1 + _latency);

		this->dma_info_queue.pop();
	} else {
		DMAEndEvent* event = rc->acquire<DMAEndEvent>(&DMAEndEvent::renew, this);
		this->scheduleEvent(event, acalsim::top->getGlobalTick() + 1);
	}
}

void DMA::dmaReadRespHandler(acalsim::Tick _when, MemReadRespPacket* _memReadRespPkt) {
	auto     rc   = acalsim::top->getRecycleContainer();
	uint32_t data = _memReadRespPkt->getData();
	instr    ins;
	bool     writeReq = false;

	if (this->curBytes > 0) {
		writeReq = true;

		int                validBytes = this->curBytes >= 4 ? 4 : this->curBytes;
		MemWriteReqPacket* pkt =
		    rc->acquire<MemWriteReqPacket>(&MemWriteReqPacket::renew, ins, SW, this->curDestAddr, data, validBytes);
		this->curBytes -= validBytes;
		this->curDestAddr += validBytes;

		AXI4BusAcqEvent* event = rc->acquire<AXI4BusAcqEvent>(
		    &AXI4BusAcqEvent::renew, dynamic_cast<AXI4Bus*>(this->getDownStream("DSAXI4Bus")), pkt);
		this->scheduleEvent(event, acalsim::top->getGlobalTick() + 1);
	}

	rc->recycle(_memReadRespPkt);

	this->curBurstCount++;
	if (this->curBurstCount == this->curBurstSize) { writeReq ? triggerNextDMA(1) : triggerNextDMA(0); }
};

void DMA::dmaEnd() {
	this->rf[0] &= ~1;
	this->rf[5] |= 1;
}
