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

#ifndef EMULATOR_INCLUDE_EVENT_MEMREQEVENT_HH_
#define EMULATOR_INCLUDE_EVENT_MEMREQEVENT_HH_

#include "ACALSim.hh"
#include "DataMemory.hh"
#include "MemPacket.hh"

class MemReadReqEvent : public acalsim::SimEvent {
public:
	MemReadReqEvent(DataMemory* _callee, MemReadReqPacket* _memReqPkt)
	    : acalsim::SimEvent("MemReadReqEvent"), callee(_callee), memReqPkt(_memReqPkt) {
		this->clearFlags(this->Managed);
	}
	~MemReadReqEvent() {}
	MemReadReqEvent() {}

	void renew(DataMemory* _callee, MemReadReqPacket* _memReqPkt) {
		this->SimEvent::renew();
		this->callee    = _callee;
		this->memReqPkt = _memReqPkt;
	}
	void process() override;

private:
	DataMemory*       callee;
	MemReadReqPacket* memReqPkt;
};

class MemWriteReqEvent : public acalsim::SimEvent {
public:
	MemWriteReqEvent(DataMemory* _callee, MemWriteReqPacket* _memReqPkt)
	    : acalsim::SimEvent("MemWriteReqEvent"), callee(_callee), memReqPkt(_memReqPkt) {
		this->clearFlags(this->Managed);
	}
	~MemWriteReqEvent() {}
	MemWriteReqEvent() {}

	void renew(DataMemory* _callee, MemWriteReqPacket* _memReqPkt) {
		this->SimEvent::renew();
		this->callee    = _callee;
		this->memReqPkt = _memReqPkt;
	}
	void process() override;

private:
	DataMemory*        callee;
	MemWriteReqPacket* memReqPkt;
};

#endif