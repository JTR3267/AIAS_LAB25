/*
 * Copyright 2023-2024 Playlab/ACAL
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "SOC.hh"

#include "event/ExecOneInstrEvent.hh"

SOC::SOC(std::string _name) : acalsim::STSimBase(_name) {
	// Get read latency for the data memory model
	CLASS_INFO << "memory_read_latency : " << acalsim::top->getParameter<acalsim::Tick>("SOC", "memory_read_latency")
	           << " Ticks";
	// Get write latency for the data memory model
	CLASS_INFO << "memory_write_latency : " << acalsim::top->getParameter<acalsim::Tick>("SOC", "memory_write_latency")
	           << " Ticks";
}

void SOC::registerModules() {
	// Get the maximal memory footprint size in the Emulator Configuration
	size_t      mem_size     = acalsim::top->getParameter<int>("Emulator", "memory_size");
	int         bus_width    = acalsim::top->getParameter<int>("SOC", "bus_width");
	int         dma_base_adr = acalsim::top->getParameter<int>("SOC", "dma_base_adr");
	int         dma_adr_size = acalsim::top->getParameter<int>("SOC", "dma_adr_size");
	BurstMode   bus_burst_mode;
	std::string config_burst_mode = acalsim::top->getParameter<std::string>("SOC", "bus_burst_mode");
	if (config_burst_mode == "FIXED") {
		bus_burst_mode = BurstMode::FIXED;
	} else if (config_burst_mode == "INCR") {
		bus_burst_mode = BurstMode::INCR;
	} else {
		CLASS_ERROR << "Wrong bus burst mode";
	}

	// Data Memory Timing Model
	this->dmem = new DataMemory("Data Memory", mem_size);

	// Instruction Set Architecture Emulator (Functional Model)
	this->isaEmulator = new Emulator("RISCV RV32I Emulator");

	// CPU Timing Model
	this->cpu = new CPU("Single-Cycle CPU Model", this->isaEmulator);

	this->axi4Bus = new AXI4Bus("AXI4 Bus", bus_width, bus_burst_mode);

	this->dma = new DMA("DMA", dma_base_adr, dma_adr_size);

	// register modules
	this->addModule(this->cpu);
	this->addModule(this->axi4Bus);
	this->addModule(this->dma);
	this->addModule(this->dmem);

	// connect modules (connected_module, master port name, slave port name)
	this->cpu->addDownStream(this->axi4Bus, "DSAXI4Bus");
	this->axi4Bus->addUpStream(this->cpu, "USCPU");
	this->dmem->addDownStream(this->axi4Bus, "DSAXI4Bus");
	this->axi4Bus->addUpStream(this->dmem, "USDmem");
	this->dma->addDownStream(this->axi4Bus, "DSAXI4Bus");
	this->axi4Bus->addUpStream(this->dma, "USDMA");
}

void SOC::simInit() {
	CLASS_INFO << name + " SOC::simInit()!";

	// Initialize the ISA Emulator
	// Parse assmebly file and initialize data memory and instruction memory
	std::string asm_file_path = acalsim::top->getParameter<std::string>("Emulator", "asm_file_path");

	this->isaEmulator->parse(asm_file_path, ((uint8_t*)this->dmem->getMemPtr()), this->cpu->getIMemPtr());
	this->isaEmulator->normalize_labels(this->cpu->getIMemPtr());

	// Initialize all child modules
	for (auto& [_, module] : this->modules) { module->init(); }

	// Inject trigger event
	auto               rc    = acalsim::top->getRecycleContainer();
	ExecOneInstrEvent* event = rc->acquire<ExecOneInstrEvent>(&ExecOneInstrEvent::renew, 1 /*id*/, this->cpu);
	this->scheduleEvent(event, acalsim::top->getGlobalTick() + 1);
}

void SOC::cleanup() {
	this->cpu->printRegfile();
	this->dmem->dumpMemory(0x2000, 0xefff, "memdump.txt");
	CLASS_INFO << "SOC::cleanup() ";
}
