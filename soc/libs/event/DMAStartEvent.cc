#include "event/DMAStartEvent.hh"

#include "DMA.hh"

DMAStartEvent::DMAStartEvent(DMA* _callee) : acalsim::SimEvent("DMAStartEvent"), callee(_callee) {}

void DMAStartEvent::renew(DMA* _callee) {
	this->acalsim::SimEvent::renew();
	this->callee = _callee;
}

void DMAStartEvent::process() { this->callee->dmaStart(); }
