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
#ifndef SOC_INCLUDE_SOCTOP_HH_
#define SOC_INCLUDE_SOCTOP_HH_

#include <string>

#include "ACALSim.hh"
#include "Emulator.hh"
#include "SOC.hh"
#include "SystemConfig.hh"

/**
 * @class SOCTop
 * @brief Top-level System-on-Chip simulation class
 * @details Manages the complete SoC simulation environment, including configuration
 *          registration, CLI argument handling, and trace control. Inherits from
 *          STSim template class specialized for SOC type.
 */
class SOCTop : public acalsim::STSim<SOC> {
public:
	/**
	 * @brief Constructor for SOCTop simulation environment
	 * @param _name Name of the simulation instance (default: "SOCTop")
	 * @param _configFile Path to configuration file (default: empty string)
	 * @details Initializes the simulation and sets up trace container with
	 *          default path "soc/trace"
	 */
	SOCTop(const std::string _name = "SOCTop", const std::string _configFile = "")
	    : acalsim::STSim<SOC>(_name, _configFile) {
		this->traceCntr.run(0, &acalsim::SimTraceContainer::setFilePath, "trace", "soc/trace");
	}

	/**
	 * @brief Registers configuration objects for the simulation
	 * @details Creates and registers two configuration objects:
	 *          1. EmulatorConfig: Configuration for the CPU emulator
	 *          2. SOCConfig: Configuration for SOC timing parameters
	 * @override Overrides base class method
	 */
	void registerConfigs() override {
		auto emuConfig = new EmulatorConfig("Emulator configuration");
		this->addConfig("Emulator", emuConfig);
		auto socConfig = new SOCConfig("SOC configuration");
		this->addConfig("SOC", socConfig);
	}

	/**
	 * @brief Registers command-line interface arguments
	 * @details Sets up CLI options for the simulation:
	 *          - --asm_file_path: Path to the assembly code file
	 * @override Overrides base class method
	 */
	void registerCLIArguments() override {
		this->addCLIOption<std::string>("--asm_file_path",                    // Option name
		                                "The file path of an assembly code",  // Description
		                                "Emulator",                           // Config section
		                                "asm_file_path"                       // Parameter name
		);
	}
};

#endif
