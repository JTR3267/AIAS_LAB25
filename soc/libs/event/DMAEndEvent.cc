#include "event/DMAEndEvent.hh"

#include "DMA.hh"

DMAEndEvent::DMAEndEvent(DMA* _callee) : acalsim::SimEvent("DMAEndEvent"), callee(_callee) {}

void DMAEndEvent::renew(DMA* _callee) {
	this->acalsim::SimEvent::renew();
	this->callee = _callee;
}

void DMAEndEvent::process() { this->callee->dmaEnd(); }
