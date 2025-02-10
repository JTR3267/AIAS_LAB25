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

#include "event/ProcessInstrEvent.hh"

#include "CPU.hh"

ProcessInstrEvent::ProcessInstrEvent(int _id, CPU* _cpu, instr& _instr)
    : acalsim::SimEvent("ProcessInstrEvent" + std::to_string(_id)), cpu(_cpu), inst(_instr) {}

void ProcessInstrEvent::renew(int _id, CPU* _cpu, instr _instr) {
	this->SimEvent::renew();
	this->cpu  = _cpu;
	this->inst = _instr;
}

void ProcessInstrEvent::process() { this->cpu->processInstr(this->inst); }
