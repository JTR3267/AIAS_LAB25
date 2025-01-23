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

#ifndef SRC_TESTEMULATOR_INCLUDE_CPUEMULATOR_HH_
#define SRC_TESTEMULATOR_INCLUDE_CPUEMULATOR_HH_

#include <string>

#include "ACALSim.hh"
using namespace acalsim;

#include "DataStruct.hh"
#include "MemPacket.hh"
#include "event/MemReqEvent.hh"
#include "event/ProcessInstrEvent.hh"

/**
 * @class A CPU model integrated with CPU ISA Emulator
 */
class CPUEmulator : public SimModule {
public:
	CPUEmulator(std::string _name) : SimModule(_name), pc(0), inst_cnt(0) {
		imem = (instr*)malloc(DATA_OFFSET * sizeof(instr) / 4);
		for (int i = 0; i < DATA_OFFSET / 4; i++) {
			imem[i].op      = UNIMPL;
			imem[i].a1.type = OPTYPE_NONE;
			imem[i].a2.type = OPTYPE_NONE;
			imem[i].a3.type = OPTYPE_NONE;
		}
		for (int i = 0; i < 32; i++) { this->rf[i] = 0; }
	}
	~CPUEmulator() { free(imem); }

	void print_regfile();

	std::string InstrToString(instr_type op);
	void        ProcessNxtInstr(Tick latency);
	void        ProcessInstr();
	instr*      getIMemPtr() { return this->imem; }
	uint32_t&   getPCRef() { return this->pc; }
	uint32_t (&getRFRef())[32] { return this->rf; }
	void       incrementInstCount() { this->inst_cnt++; }
	const int& getInstCount() { return this->inst_cnt; }

	void MemRead(int rd, instr_type op, uint32_t addr);
	void MemWrite(instr_type op, uint32_t addr, uint32_t data);
	void MemReadRespHandler(int rd, MemReadRespPacket* pkt);
	void MemWriteRespHandler();

private:
	instr*   imem;
	uint32_t rf[32];
	uint32_t pc;
	int      inst_cnt;
};

#endif  // SRC_TESTEMULATOR_INCLUDE_CPUEMULATOR_HH_
