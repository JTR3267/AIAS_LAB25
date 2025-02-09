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
#ifndef EMULATOR_INCLUDE_EMULATORTOP_HH_
#define EMULATOR_INCLUDE_EMULATORTOP_HH_

#include "ACALSim.hh"
#include "Emulator.hh"
#include "EmulatorConfig.hh"

class EmulatorTop : public acalsim::STSim<Emulator> {
public:
	EmulatorTop(const std::string _name = "EmulatorTop", const std::string _configFile = "")
	    : acalsim::STSim<Emulator>(_name, _configFile) {
		this->traceCntr.run(0, &acalsim::SimTraceContainer::setFilePath, "trace", "src/testEmulator/trace");
	}

	void registerConfigs() override {
		auto config = new EmulatorConfig("Emulator configuration");
		this->addConfig("Emulator", config);
	}
};

#endif