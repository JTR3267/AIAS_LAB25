#ifndef EMULATOR_INCLUDE_EVENT_DMASTARTEVENT_HH_
#define EMULATOR_INCLUDE_EVENT_DMASTARTEVENT_HH_

#include "ACALSim.hh"

class DMA;

class DMAStartEvent : public acalsim::SimEvent {
public:
	DMAStartEvent() = default;
	DMAStartEvent(DMA* _callee);
	virtual ~DMAStartEvent() = default;

	void renew(DMA* _callee);
	void process() override;

private:
	DMA* callee;
};

#endif
