// demo/Memory.cc

#include "Memory.hh"

Memory::Memory(std::string _name) : acalsim::SimModule(_name) { LABELED_INFO(this->getName()) << "Constructing..."; }

acalsim::Tick Memory::getProcessLatency(uint32_t _size) const { return 2; }

acalsim::Tick Memory::getRespLatency(uint32_t _size) const { return 4; }
