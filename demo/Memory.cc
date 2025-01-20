// demo/Memory.cc

#include "Memory.hh"

Memory::Memory(std::string _name) : acalsim::SimModule(_name) { LABELED_INFO(this->getName()) << "Constructing..."; }
