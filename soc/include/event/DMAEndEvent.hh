#ifndef EMULATOR_INCLUDE_EVENT_DMAENDEVENT_HH_
#define EMULATOR_INCLUDE_EVENT_DMAENDEVENT_HH_

#include "ACALSim.hh"

class DMA;

class DMAEndEvent : public acalsim::SimEvent {
public:
	DMAEndEvent() = default;
	DMAEndEvent(DMA* _callee);
	virtual ~DMAEndEvent() = default;

	void renew(DMA* _callee);
	void process() override;

private:
	DMA* callee;
};

#endif
