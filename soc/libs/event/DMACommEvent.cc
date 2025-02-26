#include "event/DMACommEvent.hh"

#include "DMA.hh"

DMACommEvent::DMACommEvent(DMA* _callee, acalsim::SimPacket* _memPkt)
    : acalsim::SimEvent("DMACommEvent"), callee(_callee), memPkt(_memPkt) {}

void DMACommEvent::renew(DMA* _callee, acalsim::SimPacket* _memPkt) {
	this->acalsim::SimEvent::renew();
	this->callee = _callee;
	this->memPkt = _memPkt;
}

void DMACommEvent::process() { this->callee->accept(acalsim::top->getGlobalTick(), *this->memPkt); }
