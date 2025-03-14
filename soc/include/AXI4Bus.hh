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

	bool axi4BusReqHandler(acalsim::Tick _when, acalsim::SimPacket* _memReqPkt, acalsim::SimModule* _sender);

	bool axi4BusReadRespHandler(acalsim::Tick _when, MemReadRespPacket* _memReadRespPkt);

	BurstMode getBurstMode() { return this->burstMode; }

	void accept(acalsim::Tick when, acalsim::SimPacket* pkt, acalsim::SimModule* sender) {
		if (auto dataPkt = dynamic_cast<MemWriteDataPacket*>(pkt)) { dataPkt->visit(when, *this, sender); }
	}

	void accept(acalsim::Tick when, acalsim::SimPacket& pkt) { pkt.visit(when, *this); }

private:
	size_t              busWidth;
	BurstMode           burstMode;
	MemWriteDataPacket* cpuWriteData;
	std::string         cpuWriteDest;
};

#endif
