/*
 * Copyright 2023-2024 Playlab/ACAL
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "DataMemory.hh"

#include "AXI4Bus.hh"
#include "event/AXI4BusAcqEvent.hh"
#include "event/MemReqDoneEvent.hh"

void DataMemory::memReqHandler(acalsim::Tick _when, acalsim::SimPacket* _memReqPkt) {
	if (auto pkt = dynamic_cast<MemReadReqPacket*>(_memReqPkt)) {
		for (int i = 0; i < pkt->getBurstSize(); i++) { this->pending_req_queue_.push({pkt, i, 0}); }
	} else if (auto pkt = dynamic_cast<MemWriteReqPacket*>(_memReqPkt)) {
		for (int i = 0; i < pkt->getBurstSize(); i++) { this->unmatch_write_pkt_queue_.push({pkt, i, 0}); }
	} else if (auto pkt = dynamic_cast<MemWriteDataPacket*>(_memReqPkt)) {
		auto queueFront = this->unmatch_write_pkt_queue_.front();
		queueFront.data = pkt->getData();
		this->pending_req_queue_.push(queueFront);
		this->unmatch_write_pkt_queue_.pop();
		acalsim::top->getRecycleContainer()->recycle(_memReqPkt);
	} else {
		CLASS_ERROR << "Wrong memory request packet type!";
	}
	if (this->is_idle_) {
		this->is_idle_ = false;
		triggerNextReq();
	} else {
		acalsim::top->addChromeTraceRecord(acalsim::ChromeTraceRecord::createDurationEvent(
		    /* ph */ "B", /* pid */ "Req-" + std::to_string(_memReqPkt->getID()), /* name */ "Req Pending",
		    /* ts */ acalsim::top->getGlobalTick(), /* cat */ "", /* tid */ "",
		    /* args */ nullptr));
	}
}

void DataMemory::triggerNextReq() {
	if (!this->pending_req_queue_.empty()) {
		auto queueFront = this->pending_req_queue_.front();
		if (auto pkt = dynamic_cast<MemReadReqPacket*>(queueFront.pkt)) {
			memReadReqHandler(pkt, queueFront.burstCount);
		} else if (auto pkt = dynamic_cast<MemWriteReqPacket*>(queueFront.pkt)) {
			memWriteReqHandler(pkt, queueFront.burstCount, queueFront.data);
		}

		acalsim::top->addChromeTraceRecord(acalsim::ChromeTraceRecord::createDurationEvent(
		    /* ph */ "E", /* pid */ "Req-" + std::to_string(queueFront.pkt->getID()), /* name */ "Req Pending",
		    /* ts */ acalsim::top->getGlobalTick(), /* cat */ "", /* tid */ "",
		    /* args */ nullptr));

		this->pending_req_queue_.pop();
	} else {
		this->is_idle_ = true;
	}
}

void DataMemory::memReadReqHandler(MemReadReqPacket* _memReqPkt, int burstCount) {
	instr      i    = _memReqPkt->getInstr();
	instr_type op   = _memReqPkt->getOP();
	uint32_t   addr = _memReqPkt->getAddr() + burstCount * 4;
	operand    a1   = _memReqPkt->getA1();

	size_t   bytes   = 0;
	uint32_t ret     = 0;
	int      latency = acalsim::top->getParameter<acalsim::Tick>("SOC", "memory_read_latency");

	switch (op) {
		case LB:
		case LBU: bytes = 1; break;
		case LH:
		case LHU: bytes = 2; break;
		case LW: bytes = 4; break;
	}

	void* data = this->readData(addr, bytes, false);

	switch (op) {
		case LB: ret = static_cast<uint32_t>(*(int8_t*)data); break;
		case LBU: ret = *(uint8_t*)data; break;
		case LH: ret = static_cast<uint32_t>(*(int16_t*)data); break;
		case LHU: ret = *(uint16_t*)data; break;
		case LW: ret = *(uint32_t*)data; break;
	}

	auto               rc              = acalsim::top->getRecycleContainer();
	MemReadRespPacket* memRespPkt      = rc->acquire<MemReadRespPacket>(&MemReadRespPacket::renew, i, op, ret, a1);
	AXI4BusAcqEvent*   axi4BusAcqEvent = rc->acquire<AXI4BusAcqEvent>(
        &AXI4BusAcqEvent::renew, dynamic_cast<AXI4Bus*>(this->getUpStream("USAXI4Bus")), memRespPkt);
	MemReqDoneEvent* memReqDoneEvent = rc->acquire<MemReqDoneEvent>(&MemReqDoneEvent::renew, this);
	this->scheduleEvent(axi4BusAcqEvent, acalsim::top->getGlobalTick() + latency + 1);
	this->scheduleEvent(memReqDoneEvent, acalsim::top->getGlobalTick() + latency);

	acalsim::top->addChromeTraceRecord(acalsim::ChromeTraceRecord::createCompleteEvent(
	    /* pid */ "Req-" + std::to_string(_memReqPkt->getID()), /* name */ "Mem Read",
	    /* ts */ acalsim::top->getGlobalTick(), /* dur */ latency, /* cat */ "", /* tid */ "",
	    /* args */ nullptr));

	acalsim::top->addChromeTraceRecord(acalsim::ChromeTraceRecord::createCompleteEvent(
	    /* pid */ "Resp-" + std::to_string(memRespPkt->getID()), /* name */ "Read Resp Bus Acq Event",
	    /* ts */ acalsim::top->getGlobalTick() + latency, /* dur */ 1, /* cat */ "", /* tid */ "",
	    /* args */ nullptr));

	if (_memReqPkt->getBurstSize() == burstCount + 1) { rc->recycle(_memReqPkt); }
}

void DataMemory::memWriteReqHandler(MemWriteReqPacket* _memReqPkt, int burstCount, int data) {
	instr      i    = _memReqPkt->getInstr();
	instr_type op   = _memReqPkt->getOP();
	uint32_t   addr = _memReqPkt->getAddr() + burstCount * 4;

	size_t bytes   = 0;
	int    latency = acalsim::top->getParameter<acalsim::Tick>("SOC", "memory_write_latency");

	switch (op) {
		case SB: bytes = 1; break;
		case SH: bytes = 2; break;
		case SW: bytes = 4; break;
	}

	switch (op) {
		case SB: {
			uint8_t val8 = static_cast<uint8_t>(data);
			this->writeData(&val8, addr, 1);
			break;
		}
		case SH: {
			uint16_t val16 = static_cast<uint16_t>(data);
			this->writeData(&val16, addr, 2);
			break;
		}
		case SW: {
			uint32_t val32 = static_cast<uint32_t>(data);
			this->writeData(&val32, addr, 4);
			break;
		}
	}

	auto             rc              = acalsim::top->getRecycleContainer();
	MemReqDoneEvent* memReqDoneEvent = rc->acquire<MemReqDoneEvent>(&MemReqDoneEvent::renew, this);
	this->scheduleEvent(memReqDoneEvent, acalsim::top->getGlobalTick() + latency);

	acalsim::top->addChromeTraceRecord(acalsim::ChromeTraceRecord::createCompleteEvent(
	    /* pid */ "Req-" + std::to_string(_memReqPkt->getID()), /* name */ "Mem Write",
	    /* ts */ acalsim::top->getGlobalTick(), /* dur */ latency, /* cat */ "", /* tid */ "",
	    /* args */ nullptr));

	if (_memReqPkt->getBurstSize() == burstCount + 1) { rc->recycle(_memReqPkt); }
}
