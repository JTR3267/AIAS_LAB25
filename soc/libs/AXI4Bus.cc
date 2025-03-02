#include "AXI4Bus.hh"

#include "CPU.hh"
#include "DMA.hh"
#include "DataMemory.hh"
#include "MemPacket.hh"
#include "event/CPUCommEvent.hh"
#include "event/DMACommEvent.hh"
#include "event/DMemCommEvent.hh"

AXI4Bus::AXI4Bus(std::string _name, size_t _busWidth, BurstMode _burstMode)
    : acalsim::SimModule(_name), busWidth(_busWidth), burstMode(_burstMode) {}

bool AXI4Bus::axi4BusReqHandler(acalsim::Tick _when, MemReqPacket* _memReqPkt) {
	auto               adr          = _memReqPkt->getAddr();
	auto               rc           = acalsim::top->getRecycleContainer();
	auto               dma_base_adr = static_cast<DMA*>(this->getUpStream("USDMA"))->getBaseAddr();
	acalsim::SimEvent* event;

	std::string sender   = dynamic_cast<DMA*>(_memReqPkt->getSender()) ? "DMA" : "CPU";
	std::string reqType  = dynamic_cast<MemReadReqPacket*>(_memReqPkt) ? " Read Req-" : " Write Req-";
	std::string receiver = adr >= dma_base_adr ? "DMA" : "Mem";

	acalsim::top->addChromeTraceRecord(acalsim::ChromeTraceRecord::createCompleteEvent(
	    /* pid */ sender + reqType + std::to_string(_memReqPkt->getID()),
	    /* name */ receiver + " Transfer Event",
	    /* ts */ acalsim::top->getGlobalTick(), /* dur */ 1, /* cat */ "", /* tid */ "",
	    /* args */ nullptr));

	if (adr >= dma_base_adr) {
		event =
		    rc->acquire<DMACommEvent>(&DMACommEvent::renew, dynamic_cast<DMA*>(this->getUpStream("USDMA")), _memReqPkt);
	} else {
		if (auto pkt = dynamic_cast<MemReadReqPacket*>(_memReqPkt)) {
			int burstSize = pkt->getBurstSize();
			if ((this->burstMode == BurstMode::FIXED && burstSize > 1) ||
			    (this->burstMode == BurstMode::INCR && burstSize != 1 && burstSize != 2 && burstSize != 4)) {
				CLASS_ERROR << "Wrong burst size!";
			}
		}
		event = rc->acquire<DMemCommEvent>(&DMemCommEvent::renew,
		                                   dynamic_cast<DataMemory*>(this->getUpStream("USDmem")), _memReqPkt);
	}

	this->scheduleEvent(event, acalsim::top->getGlobalTick() + 1);
	return false;
}

bool AXI4Bus::axi4BusReadRespHandler(acalsim::Tick _when, MemReadRespPacket* _memReadRespPkt) {
	auto               rc = acalsim::top->getRecycleContainer();
	acalsim::SimEvent* event;

	if (auto dma = dynamic_cast<DMA*>(_memReadRespPkt->getReceiver())) {
		acalsim::top->addChromeTraceRecord(acalsim::ChromeTraceRecord::createCompleteEvent(
		    /* pid */ "Mem Read Resp-" + std::to_string(_memReadRespPkt->getID()), /* name */ "DMA Transfer Event",
		    /* ts */ acalsim::top->getGlobalTick(), /* dur */ 1, /* cat */ "", /* tid */ "",
		    /* args */ nullptr));

		event = rc->acquire<DMACommEvent>(&DMACommEvent::renew, dynamic_cast<DMA*>(this->getUpStream("USDMA")),
		                                  _memReadRespPkt);
	} else if (auto cpu = dynamic_cast<CPU*>(_memReadRespPkt->getReceiver())) {
		std::string sender = dynamic_cast<DMA*>(_memReadRespPkt->getSender()) ? "DMA" : "Mem";
		acalsim::top->addChromeTraceRecord(acalsim::ChromeTraceRecord::createCompleteEvent(
		    /* pid */ sender + " Read Resp-" + std::to_string(_memReadRespPkt->getID()),
		    /* name */ "CPU Transfer Event",
		    /* ts */ acalsim::top->getGlobalTick(), /* dur */ 1, /* cat */ "", /* tid */ "",
		    /* args */ nullptr));

		event = rc->acquire<CPUCommEvent>(&CPUCommEvent::renew, dynamic_cast<CPU*>(this->getUpStream("USCPU")),
		                                  _memReadRespPkt);
	}

	this->scheduleEvent(event, acalsim::top->getGlobalTick() + 1);

	return false;
}
