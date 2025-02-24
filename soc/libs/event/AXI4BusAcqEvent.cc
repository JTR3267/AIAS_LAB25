#include "event/AXI4BusAcqEvent.hh"

#include "AXI4Bus.hh"

AXI4BusAcqEvent::AXI4BusAcqEvent(AXI4Bus* _callee, acalsim::SimPacket* _memPkt)
    : acalsim::SimEvent("AXI4BusAcqEvent"), callee(_callee), memPkt(_memPkt) {}

void AXI4BusAcqEvent::renew(AXI4Bus* _callee, acalsim::SimPacket* _memPkt) {
	this->acalsim::SimEvent::renew();
	this->callee = _callee;
	this->memPkt = _memPkt;
}

void AXI4BusAcqEvent::process() { this->callee->accept(acalsim::top->getGlobalTick(), *this->memPkt); }
