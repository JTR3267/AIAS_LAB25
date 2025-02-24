#include "event/MemRespEvent.hh"

#include "CPU.hh"

MemRespEvent::MemRespEvent(CPU* _callee, acalsim::SimPacket* _memPkt)
    : acalsim::SimEvent("MemRespEvent"), callee(_callee), memPkt(_memPkt) {}

void MemRespEvent::renew(CPU* _callee, acalsim::SimPacket* _memPkt) {
	this->acalsim::SimEvent::renew();
	this->callee = _callee;
	this->memPkt = _memPkt;
}

void MemRespEvent::process() { this->callee->accept(acalsim::top->getGlobalTick(), *this->memPkt); }
