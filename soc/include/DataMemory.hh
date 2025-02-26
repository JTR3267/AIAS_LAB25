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

#ifndef SOC_INCLUDE_DATAMEMORY_HH_
#define SOC_INCLUDE_DATAMEMORY_HH_

#include <queue>
#include <string>

#include "ACALSim.hh"
#include "BaseMemory.hh"
#include "DataStruct.hh"
#include "MemPacket.hh"

typedef struct ReqInfo {
	MemReqPacket* memReqPkt;
	int           burstCount;
} ReqInfo;

/**
 * @class DataMemory
 * @brief Data memory module for the system
 * @details Implements a memory module that handles both read and write operations
 *          Inherits from SimModule for simulation functionality and BaseMemory
 *          for basic memory operations
 */
class DataMemory : public acalsim::SimModule, public BaseMemory {
public:
	/**
	 * @brief Constructor for DataMemory
	 * @param _name Name identifier for the memory module
	 * @param _size Size of the memory in bytes
	 */
	DataMemory(std::string _name, size_t _size) : acalsim::SimModule(_name), BaseMemory(_size) {}

	/**
	 * @brief Virtual destructor
	 */
	virtual ~DataMemory() {}

	/**
	 * @brief Handles memory read request packets
	 * @param _when Simulation time tick when the request was received
	 * @param _memReqPkt Pointer to the memory read request packet
	 * @details Processes incoming read requests and generates appropriate responses
	 */
	void memReadReqHandler(MemReadReqPacket* _memReadReqPkt, int burstCount);

	/**
	 * @brief Handles memory write request packets
	 * @param _when Simulation time tick when the request was received
	 * @param _memReqPkt Pointer to the memory write request packet
	 * @details Processes incoming write requests and updates memory contents
	 */
	void memWriteReqHandler(MemWriteReqPacket* _memWriteReqPkt);

	void memReqHandler(acalsim::Tick _when, MemReqPacket* _memReqPkt);

	void triggerNextReq();

private:
	std::queue<ReqInfo> pending_req_queue;
	bool                is_idle = true;
};

#endif
