#include "event/CPUCommEvent.hh"

#include "CPU.hh"

CPUCommEvent::CPUCommEvent(CPU* _callee, MemReadRespPacket* _memReadRespPkt)
    : acalsim::SimEvent("CPUCommEvent"), callee(_callee), memReadRespPkt(_memReadRespPkt) {}

void CPUCommEvent::renew(CPU* _callee, MemReadRespPacket* _memReadRespPkt) {
	this->acalsim::SimEvent::renew();
	this->callee         = _callee;
	this->memReadRespPkt = _memReadRespPkt;
}

void CPUCommEvent::process() { this->callee->accept(acalsim::top->getGlobalTick(), *this->memReadRespPkt); }
