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

#ifndef SRC_TESTEMULATOR_INCLUDE_DATAMEMORY_HH_
#define SRC_TESTEMULATOR_INCLUDE_DATAMEMORY_HH_

#include <string>

#include "ACALSim.hh"
#include "BaseMemory.hh"
#include "DataStruct.hh"
#include "MemPacket.hh"

using namespace acalsim;

/**
 * @class A simple memory model
 */
class DataMemory : public SimModule, public BaseMemory {
public:
	/**
	 * @brief Construct a new Memory object
	 *
	 * @param _name The name of memory.
	 * @param _size The size of memory.
	 */
	DataMemory(std::string _name, size_t _size) : SimModule(_name), BaseMemory(_size) {}

	~DataMemory() {}

	void initMem(char* _data, size_t _size) { this->writeData(_data, (uint32_t)0x0, _size); }

	void init() override {
		size_t mem_size = this->getSize();
		CLASS_INFO << mem_size;
		char* zeroBuf = new char[mem_size]();
		this->initMem(zeroBuf, mem_size);

		delete[] zeroBuf;
	}

	void memReadReqHandler(Tick _when, MemReadReqPacket* _memReqPkt);

	void memWriteReqHandler(Tick _when, MemWriteReqPacket* _memReqPkt);
};
#endif  // SRC_TESTEMULATOR_INCLUDE_DATAMEMORY_HH_
