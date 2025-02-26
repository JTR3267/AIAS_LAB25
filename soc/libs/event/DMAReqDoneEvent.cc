#include "DMA.hh"
#include "event/DMAReqDoneEvent.hh"

DMAReqDoneEvent::DMAReqDoneEvent(DMA* _callee) : acalsim::SimEvent("DMAReqDoneEvent"), callee(_callee) {}

void DMAReqDoneEvent::renew(DMA* _callee) {
	this->acalsim::SimEvent::renew();
	this->callee = _callee;
}

void DMAReqDoneEvent::process() { this->callee->triggerNextReq(); }
