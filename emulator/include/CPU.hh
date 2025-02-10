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

#ifndef EMULATOR_INCLUDE_CPU_HH_
#define EMULATOR_INCLUDE_CPU_HH_

#include <string>

#include "ACALSim.hh"
#include "DataStruct.hh"
#include "MemPacket.hh"

/**
 * @class A CPU model integrated with CPU ISA Emulator
 */
class CPU : public acalsim::SimModule {
public:
	CPU(std::string _name);
	virtual ~CPU() { free(this->imem); }

	void processNxtInstr();
	void processInstr(const instr& _i, uint32_t _mem_data = 0);
	void memRead(instr_type _op, uint32_t _addr);
	void memWrite(instr_type _op, uint32_t _addr, uint32_t _data);
	void memReadRespHandler(MemReadRespPacket* _pkt);
	void memWriteRespHandler();

	inline instr* getIMemPtr() const { return this->imem; }
	void          printRegfile() const;

protected:
	instr             fetchInstr(uint32_t _pc) const;
	std::string       instrToString(instr_type _op) const;
	inline void       incrementInstCount() { this->inst_cnt++; }
	inline const int& getInstCount() const { return this->inst_cnt; }

private:
	instr*   imem;
	uint32_t rf[32];
	uint32_t pc;
	int      inst_cnt;
};

#endif
