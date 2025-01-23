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

#include "ACALSim.hh"
using namespace acalsim;

#include "System.hh"
#include "SystemConfig.hh"

class SystemTop : public STSim<System> {
public:
	SystemTop(const std::string _name = "SystemSim", const std::string _configFile = "")
	    : STSim<System>(_name, _configFile) {
		this->traceCntr.run(0, &SimTraceContainer::setFilePath, "trace", "src/testEmulator/trace");
	}

	void registerConfigs() override {
		/* 1. instantiate "SystemConfig" in constructor of simulator. (Use long name to describe ConfigBase) */
		auto config = new system_config::SystemConfig("System configuration");

		/* 2. add "SystemConfig" into configContainer (Use short name to index ConfigBase) */
		this->addConfig("System", config);
	}
};
