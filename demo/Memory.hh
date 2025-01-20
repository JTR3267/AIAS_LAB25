// demo/Memory.hh

#ifndef DEMO_MEMORY_HH_
#define DEMO_MEMORY_HH_

#include <string>

#include "ACALSim.hh"

class Memory : public acalsim::SimModule {
public:
	explicit Memory(std::string _name = "Memory");
};

#endif  // DEMO_MEMORY_HH_
