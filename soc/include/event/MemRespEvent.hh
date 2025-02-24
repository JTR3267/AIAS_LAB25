#ifndef EMULATOR_INCLUDE_EVENT_MEMRESPEVENT_HH_
#define EMULATOR_INCLUDE_EVENT_MEMRESPEVENT_HH_

#include "ACALSim.hh"

class CPU;

class MemRespEvent : public acalsim::SimEvent {
public:
	MemRespEvent() = default;
	MemRespEvent(CPU* _callee, acalsim::SimPacket* _memPkt);
	virtual ~MemRespEvent() = default;

	void renew(CPU* _callee, acalsim::SimPacket* _memPkt);
	void process() override;

private:
	CPU*                callee;
	acalsim::SimPacket* memPkt;
};

#endif
