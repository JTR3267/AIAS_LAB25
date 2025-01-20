// demo/TrafficGenerator.hh

#ifndef DEMO_TRAFFICGENERATOR_HH_
#define DEMO_TRAFFICGENERATOR_HH_

#include <string>

#include "ACALSim.hh"

class TrafficGenerator : public acalsim::SimModule {
public:
	explicit TrafficGenerator(std::string _name = "TrafficGenerator");
};

#endif  // DEMO_TRAFFICGENERATOR_HH_
