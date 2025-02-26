#ifndef EMULATOR_INCLUDE_EVENT_DMACOMMEVENT_HH_
#define EMULATOR_INCLUDE_EVENT_DMACOMMEVENT_HH_

#include "ACALSim.hh"

class DMA;

class DMACommEvent : public acalsim::SimEvent {
public:
	DMACommEvent() = default;
	DMACommEvent(DMA* _callee, acalsim::SimPacket* _memPkt);
	virtual ~DMACommEvent() = default;

	void renew(DMA* _callee, acalsim::SimPacket* _memPkt);
	void process() override;

private:
	DMA*                callee;
	acalsim::SimPacket* memPkt;
};

#endif
