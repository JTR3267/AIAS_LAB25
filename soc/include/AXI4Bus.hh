#ifndef SOC_INCLUDE_AXI4BUS_HH_
#define SOC_INCLUDE_AXI4BUS_HH_

#include <string>

#include "ACALSim.hh"
#include "MemPacket.hh"

enum BurstMode { FIXED, INCR };

class AXI4Bus : public acalsim::SimModule {
public:
	AXI4Bus(std::string _name, size_t _busWidth, BurstMode _burstMode);

	virtual ~AXI4Bus() {}

	bool memReqHandler(acalsim::Tick _when, acalsim::SimPacket* _memReqPkt);

	bool memRespHandler(acalsim::Tick _when, acalsim::SimPacket* _memRespPkt);

private:
	size_t    busWidth;
	BurstMode burstMode;
};

#endif
