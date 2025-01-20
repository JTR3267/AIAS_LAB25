// demo/TrafficGeneraotr.cc

#include "TrafficGenerator.hh"

TrafficGenerator::TrafficGenerator(std::string _name) : acalsim::SimModule(_name) {
	LABELED_INFO(this->getName()) << "Constructing...";
}
