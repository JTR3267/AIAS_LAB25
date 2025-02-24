#ifndef EMULATOR_INCLUDE_EVENT_AXI4BUSACQEVENT_HH_
#define EMULATOR_INCLUDE_EVENT_AXI4BUSACQEVENT_HH_

#include "ACALSim.hh"

class AXI4Bus;

class AXI4BusAcqEvent : public acalsim::SimEvent {
public:
	AXI4BusAcqEvent() = default;
	AXI4BusAcqEvent(AXI4Bus* _callee, acalsim::SimPacket* _memPkt);
	virtual ~AXI4BusAcqEvent() = default;

	void renew(AXI4Bus* _callee, acalsim::SimPacket* _memPkt);
	void process() override;

private:
	AXI4Bus*            callee;
	acalsim::SimPacket* memPkt;
};

#endif
