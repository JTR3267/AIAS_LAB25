// demo/TrafficGenerator.hh

#ifndef DEMO_TRAFFICGENERATOR_HH_
#define DEMO_TRAFFICGENERATOR_HH_

#include <cstdint>
#include <string>
#include <vector>

#include "ACALSim.hh"

struct RequestInfo {
	acalsim::Tick issue_tick_ = -1;
	uint32_t      addr_       = -1;
	uint32_t      size_       = 0;
};

class TrafficGenerator : public acalsim::SimModule {
public:
	explicit TrafficGenerator(std::string _name = "TrafficGenerator");

	std::vector<RequestInfo> genRequests() const;

	bool isAllReqCompleted() const;
};

#endif  // DEMO_TRAFFICGENERATOR_HH_
