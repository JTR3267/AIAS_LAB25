#ifndef EMULATOR_INCLUDE_EVENT_CPUCOMMEVENT_HH_
#define EMULATOR_INCLUDE_EVENT_CPUCOMMEVENT_HH_

#include "ACALSim.hh"
#include "MemPacket.hh"

class CPU;

class CPUCommEvent : public acalsim::SimEvent {
public:
	CPUCommEvent() = default;
	CPUCommEvent(CPU* _callee, MemReadRespPacket* _memReadRespPkt);
	virtual ~CPUCommEvent() = default;

	void renew(CPU* _callee, MemReadRespPacket* _memReadRespPkt);
	void process() override;

private:
	CPU*               callee;
	MemReadRespPacket* memReadRespPkt;
};

#endif
