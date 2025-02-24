#include "AXI4Bus.hh"

#include "CPU.hh"
#include "DataMemory.hh"
#include "event/MemReqEvent.hh"
#include "event/MemRespEvent.hh"

AXI4Bus::AXI4Bus(std::string _name, size_t _busWidth, BurstMode _burstMode)
    : acalsim::SimModule(_name), busWidth(_busWidth), burstMode(_burstMode) {}

bool AXI4Bus::memReqHandler(acalsim::Tick _when, acalsim::SimPacket* _memReqPkt) {
	if (auto pkt = dynamic_cast<MemReadReqPacket*>(_memReqPkt)) {
		int burstSize = pkt->getBurstSize();
		if ((this->burstMode == BurstMode::FIXED && burstSize > 1) ||
		    (this->burstMode == BurstMode::INCR && burstSize != 1 && burstSize != 2 && burstSize != 4)) {
			CLASS_ERROR << "Wrong burst size!";
		}
	}

	auto         rc    = acalsim::top->getRecycleContainer();
	MemReqEvent* event = rc->acquire<MemReqEvent>(&MemReqEvent::renew,
	                                              dynamic_cast<DataMemory*>(this->getDownStream("DSDmem")), _memReqPkt);
	this->scheduleEvent(event, acalsim::top->getGlobalTick() + 1);

	return false;
}

bool AXI4Bus::memRespHandler(acalsim::Tick _when, acalsim::SimPacket* _memRespPkt) {
	auto          rc = acalsim::top->getRecycleContainer();
	MemRespEvent* event =
	    rc->acquire<MemRespEvent>(&MemRespEvent::renew, dynamic_cast<CPU*>(this->getUpStream("USCPU")), _memRespPkt);
	this->scheduleEvent(event, acalsim::top->getGlobalTick() + 1);

	return false;
}
