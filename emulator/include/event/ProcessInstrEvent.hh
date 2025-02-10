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

#ifndef EMULATOR_INCLUDE_EVENT_PROCESSINSTREVENT_HH_
#define EMULATOR_INCLUDE_EVENT_PROCESSINSTREVENT_HH_

#include "ACALSim.hh"
#include "DataStruct.hh"

class CPU;

class ProcessInstrEvent : public acalsim::SimEvent {
public:
	ProcessInstrEvent() = default;
	ProcessInstrEvent(int _id, CPU* _cpu, instr& _instr);
	virtual ~ProcessInstrEvent() = default;

	void renew(int _id, CPU* _cpu, instr _instr);
	void process() override;

private:
	CPU*  cpu;
	instr inst;
};

#endif
