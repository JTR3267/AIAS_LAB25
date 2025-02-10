
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

#ifndef EMULATOR_INCLUDE_EMULATORCONFIG_HH_
#define EMULATOR_INCLUDE_EMULATORCONFIG_HH_

#include <string>

#include "ACALSim.hh"

using json = nlohmann::json;

class EmulatorConfig : public acalsim::SimConfig {
public:
	EmulatorConfig(const std::string& _name) : acalsim::SimConfig(_name) {
		this->addParameter<int>("memory_size", 256, acalsim::ParamType::INT);
		this->addParameter<int>("data_offset", 8192, acalsim::ParamType::INT);
		this->addParameter<int>("text_offset", 0, acalsim::ParamType::INT);
		this->addParameter<int>("max_label_count", 128, acalsim::ParamType::INT);
		this->addParameter<int>("max_label_len", 32, acalsim::ParamType::INT);
		this->addParameter<int>("max_src_len", 1048576, acalsim::ParamType::INT);
		this->addParameter<std::string>("asm_file_path", "", acalsim::ParamType::STRING);
		this->addParameter<acalsim::Tick>("memory_read_latency", 1, acalsim::ParamType::TICK);
		this->addParameter<acalsim::Tick>("memory_write_latency", 1, acalsim::ParamType::TICK);
	}
	~EmulatorConfig() {}
};

#endif
