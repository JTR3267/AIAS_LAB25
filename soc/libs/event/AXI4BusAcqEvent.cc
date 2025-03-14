#include "event/AXI4BusAcqEvent.hh"

#include "AXI4Bus.hh"
#include "MemPacket.hh"

AXI4BusAcqEvent::AXI4BusAcqEvent(AXI4Bus* _callee, acalsim::SimPacket* _memPkt, acalsim::SimModule* _sender)
    : acalsim::SimEvent("AXI4BusAcqEvent"), callee(_callee), memPkt(_memPkt), sender(_sender) {}

void AXI4BusAcqEvent::renew(AXI4Bus* _callee, acalsim::SimPacket* _memPkt, acalsim::SimModule* _sender) {
	this->acalsim::SimEvent::renew();
	this->callee = _callee;
	this->memPkt = _memPkt;
	this->sender = _sender;
}

void AXI4BusAcqEvent::process() {
	if (dynamic_cast<MemWriteDataPacket*>(this->memPkt)) {
		this->callee->accept(acalsim::top->getGlobalTick(), this->memPkt, this->sender);
	} else {
		this->callee->accept(acalsim::top->getGlobalTick(), *this->memPkt);
	}
}
