// demo/Memory.hh

#ifndef DEMO_MEMORY_HH_
#define DEMO_MEMORY_HH_

#include <string>

#include "ACALSim.hh"

class Memory : public acalsim::SimModule {
public:
	explicit Memory(std::string _name = "Memory");

	acalsim::Tick getProcessLatency(uint32_t _size) const;
	acalsim::Tick getRespLatency(uint32_t _size) const;
};

#endif  // DEMO_MEMORY_HH_
