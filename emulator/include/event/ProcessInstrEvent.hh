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

#ifndef SRC_TESTEMULATOR_INCLUDE_PROCESSINSTREVENT_HH_
#define SRC_TESTEMULATOR_INCLUDE_PROCESSINSTREVENT_HH_

#include "ACALSim.hh"
using namespace acalsim;

#include "CPUEmulator.hh"
#include "DataStruct.hh"

class ProcessInstrEvent : public SimEvent {
public:
	ProcessInstrEvent(int _id, SimModule* _sim, instr& _instr)
	    : SimEvent("ProcessInstrEvent" + std::to_string(_id)), sim(_sim), inst(_instr) {
		this->clearFlags(Managed);
	}
	~ProcessInstrEvent() {}
	ProcessInstrEvent() {}

	void renew(int _id, SimModule* _sim, instr _instr) {
		this->sim  = _sim;
		this->inst = _instr;
	}
	void process() override;

private:
	SimModule* sim;
	instr      inst;
};

#endif