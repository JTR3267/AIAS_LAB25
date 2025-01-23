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

#ifndef SRC_TESTEMULATOR_INCLUDE_SYSTEM_HH_
#define SRC_TESTEMULATOR_INCLUDE_SYSTEM_HH_

#include <memory>

#include "ACALSim.hh"
using namespace acalsim;

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CPUEmulator.hh"
#include "DataMemory.hh"
#include "DataStruct.hh"

class System : public STSimBase {
public:
	System(std::string name = "System Simulator");
	~System() {}
	void registerModules() override;

	void simInit() override {
		CLASS_INFO << name + " System::simInit()!";

		// Initialize all child modules
		for (auto& [_, module] : this->modules) { module->init(); }

		// Parse assmebly file and initialize data memory and instruction memory
		std::string asm_file_path = top->getParameter<std::string>("System", "asm_file_path");

		this->parse(asm_file_path, ((uint8_t*)this->dmem->getMemPtr()), this->cpu->getIMemPtr(), this->memoff,
		            this->labels, this->label_count, &(this->src));
		this->normalize_labels(this->cpu->getIMemPtr(), this->labels, this->label_count, &(this->src));

		this->cpu->ProcessNxtInstr(1);
	}

	uint32_t   label_addr(char* label, label_loc* labels, int label_count, int orig_line);
	void       append_source(const char* op, const char* a1, const char* a2, const char* a3, source* src, instr* i);
	int        parse_reg(char* tok, int line, bool strict = true);
	uint32_t   parse_imm(char* tok, int bits, int line, bool strict = true);
	void       parse_mem(char* tok, int* reg, uint32_t* imm, int bits, int line);
	int        parse_assembler_directive(int line, char* ftok, uint8_t* mem, int memoff);
	int        parse_instr(int line, char* ftok, instr* imem, int memoff, label_loc* labels, source* src);
	instr_type parse_instr(char* tok);
	int parse_pseudoinstructions(int line, char* ftok, instr* imem, int ioff, label_loc* labels, char* o1, char* o2,
	                             char* o3, char* o4, source* src);
	int parse_data_element(int line, int size, uint8_t* mem, int offset);

	void     print_syntax_error(int line, const char* msg);
	bool     streq(char* s, const char* q);
	uint32_t signextend(uint32_t in, int bits);

	void parse(const std::string& file_path, uint8_t* mem, instr* imem, int& memoff, label_loc* labels,
	           int& label_count, source* src);
	void normalize_labels(instr* imem, label_loc* labels, int label_count, source* src);

	void cleanup() override { CLASS_INFO << "System::cleanup() " + name; }

private:
	CPUEmulator* cpu;
	DataMemory*  dmem;
	label_loc*   labels;
	int          label_count;
	int          memoff;
	source       src;
};

#endif  // SRC_TESTEMULATOR_INCLUDE_SYSTEM_HH_
