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

#ifndef SRC_TESTEMULATOR_INCLUDE_MEMREQEVENT_HH_
#define SRC_TESTEMULATOR_INCLUDE_MEMREQEVENT_HH_

#include "ACALSim.hh"
using namespace acalsim;

#include "DataMemory.hh"
#include "MemPacket.hh"

class MemReadReqEvent : public SimEvent {
public:
	MemReadReqEvent(SimModule*        _callee,    // pointer of callee
	                MemReadReqPacket* _memReqPkt  // The packet for the downstream callee to process
	                )
	    : SimEvent("MemReadReqEvent"), callee(_callee), memReqPkt(_memReqPkt) {
		this->clearFlags(this->Managed);
	}
	~MemReadReqEvent() {}
	MemReadReqEvent() {}

	void renew(SimModule* _callee, MemReadReqPacket* _memReqPkt) {
		this->callee    = _callee;
		this->memReqPkt = _memReqPkt;
	}
	void process() override;

private:
	SimModule*        callee;
	MemReadReqPacket* memReqPkt;
};

class MemWriteReqEvent : public SimEvent {
public:
	MemWriteReqEvent(SimModule*         _callee,    // pointer of callee
	                 MemWriteReqPacket* _memReqPkt  // The packet for the downstream callee to process
	                 )
	    : SimEvent("MemWriteReqEvent"), callee(_callee), memReqPkt(_memReqPkt) {
		this->clearFlags(this->Managed);
	}
	~MemWriteReqEvent() {}
	MemWriteReqEvent() {}

	void renew(SimModule* _callee, MemWriteReqPacket* _memReqPkt) {
		this->callee    = _callee;
		this->memReqPkt = _memReqPkt;
	}
	void process() override;

private:
	SimModule*         callee;
	MemWriteReqPacket* memReqPkt;
};

#endif