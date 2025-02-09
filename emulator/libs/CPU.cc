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

#include "CPU.hh"

#include <iomanip>
#include <sstream>

void CPU::printRegfile() {
	std::ostringstream oss;

	oss << "CPU ISA emulation is done\n";
	for (int i = 0; i < 32; i++) {
		oss << "x" << std::setw(2) << std::setfill('0') << std::dec << i << ":0x";

		oss << std::setw(8) << std::setfill('0') << std::hex << this->rf[i] << " ";

		if ((i + 1) % 8 == 0) { oss << "\n"; }
	}

	oss << "\n";

	CLASS_INFO << oss.str();
}

void CPU::processNxtInstr() {
	instr i = this->fetchInstr(this->pc);

	switch (i.op) {
		case LB:
		case LBU:
		case LH:
		case LHU:
		case LW: this->memRead(i.a1.reg, i.op, this->rf[i.a2.reg] + i.a3.imm); break;

		case SB:
		case SH:
		case SW: this->memWrite(i.op, this->rf[i.a2.reg] + i.a3.imm, this->rf[i.a1.reg]); break;
		default: {
			auto               rc = acalsim::top->getRecycleContainer();
			ProcessInstrEvent* event =
			    rc->acquire<ProcessInstrEvent>(&ProcessInstrEvent::renew, this->inst_cnt, this, i);
			this->scheduleEvent(event, acalsim::top->getGlobalTick() + 1);
		} break;
	}
}

void CPU::memRead(int _rd, instr_type _op, uint32_t _addr) {
	auto rc       = acalsim::top->getRecycleContainer();
	auto callback = [this, _rd](MemReadRespPacket* _pkt) { this->memReadRespHandler(_rd, _pkt); };

	MemReadReqPacket* pkt = rc->acquire<MemReadReqPacket>(&MemReadReqPacket::renew, callback, _op, _addr);

	MemReadReqEvent* event = rc->acquire<MemReadReqEvent>(
	    &MemReadReqEvent::renew, dynamic_cast<DataMemory*>(this->getDownStream("DSDmem")), pkt);
	int latency = acalsim::top->getParameter<acalsim::Tick>("Emulator", "memory_read_latency");
	this->scheduleEvent(event, acalsim::top->getGlobalTick() + latency);
}

void CPU::memWrite(instr_type _op, uint32_t _addr, uint32_t _data) {
	auto rc       = acalsim::top->getRecycleContainer();
	auto callback = [this]() { this->memWriteRespHandler(); };

	MemWriteReqPacket* pkt   = rc->acquire<MemWriteReqPacket>(&MemWriteReqPacket::renew, callback, _op, _addr, _data);
	MemWriteReqEvent*  event = rc->acquire<MemWriteReqEvent>(
        &MemWriteReqEvent::renew, dynamic_cast<DataMemory*>(this->getDownStream("DSDmem")), pkt);
	int latency = acalsim::top->getParameter<acalsim::Tick>("Emulator", "memory_write_latency");
	this->scheduleEvent(event, acalsim::top->getGlobalTick() + latency);
}

std::string CPU::instrToString(instr_type _op) {
	switch (_op) {
		case UNIMPL: return "UNIMPL";

		// R-type
		case ADD: return "ADD";
		case AND: return "AND";
		case OR: return "OR";
		case XOR: return "XOR";
		case SUB: return "SUB";
		case SLL: return "SLL";
		case SRL: return "SRL";
		case SRA: return "SRA";
		case SLT: return "SLT";
		case SLTU: return "SLTU";

		// I-type
		case ADDI: return "ADDI";
		case ANDI: return "ANDI";
		case ORI: return "ORI";
		case XORI: return "XORI";
		case SLLI: return "SLLI";
		case SRLI: return "SRLI";
		case SRAI: return "SRAI";
		case SLTI: return "SLTI";
		case SLTIU: return "SLTIU";

		// Load
		case LB: return "LB";
		case LBU: return "LBU";
		case LH: return "LH";
		case LHU: return "LHU";
		case LW: return "LW";

		// Store
		case SB: return "SB";
		case SH: return "SH";
		case SW: return "SW";

		// Branch
		case BEQ: return "BEQ";
		case BNE: return "BNE";
		case BGE: return "BGE";
		case BGEU: return "BGEU";
		case BLT: return "BLT";
		case BLTU: return "BLTU";

		// Jump
		case JAL: return "JAL";
		case JALR: return "JALR";

		// Upper / Immediate
		case AUIPC: return "AUIPC";
		case LUI: return "LUI";

		// Special
		case HCF: return "HCF";

		default: return "UNKNOWN";
	}
}

instr CPU::fetchInstr(uint32_t _pc) {
	auto     rf_ref = this->rf;
	uint32_t iid    = _pc / 4;
	instr    i      = this->imem[iid];
	return i;
}

void CPU::processInstr(uint32_t _rd = -1, uint32_t _value = 0) {
	if (_rd != -1) { this->rf[_rd] = _value; }
	this->pc += 4;
}

void CPU::processInstr(const instr& _i) {
	auto rf_ref = this->rf;
	this->incrementInstCount();
	int pc_next = this->pc + 4;

	enum NxtState { PROCESS, MEMREQ, END };

	NxtState nxt_state = PROCESS;

	switch (_i.op) {
			// instruction added
			// case MUL: rf[i.a1.reg] = rf[i.a2.reg] * rf[i.a3.reg]; break;
			//*****************

		case ADD: rf_ref[_i.a1.reg] = rf_ref[_i.a2.reg] + rf_ref[_i.a3.reg]; break;
		case SUB: rf_ref[_i.a1.reg] = rf_ref[_i.a2.reg] - rf_ref[_i.a3.reg]; break;
		case SLT: rf_ref[_i.a1.reg] = (*(int32_t*)&rf_ref[_i.a2.reg]) < (*(int32_t*)&rf_ref[_i.a3.reg]) ? 1 : 0; break;
		case SLTU: rf_ref[_i.a1.reg] = rf_ref[_i.a2.reg] + rf_ref[_i.a3.reg]; break;
		case AND: rf_ref[_i.a1.reg] = rf_ref[_i.a2.reg] & rf_ref[_i.a3.reg]; break;
		case OR: rf_ref[_i.a1.reg] = rf_ref[_i.a2.reg] | rf_ref[_i.a3.reg]; break;
		case XOR: rf_ref[_i.a1.reg] = rf_ref[_i.a2.reg] ^ rf_ref[_i.a3.reg]; break;
		case SLL: rf_ref[_i.a1.reg] = rf_ref[_i.a2.reg] << rf_ref[_i.a3.reg]; break;
		case SRL: rf_ref[_i.a1.reg] = rf_ref[_i.a2.reg] >> rf_ref[_i.a3.reg]; break;
		case SRA: rf_ref[_i.a1.reg] = (*(int32_t*)&rf_ref[_i.a2.reg]) >> rf_ref[_i.a3.reg]; break;

		case ADDI: rf_ref[_i.a1.reg] = rf_ref[_i.a2.reg] + _i.a3.imm; break;
		case SLTI: rf_ref[_i.a1.reg] = (*(int32_t*)&rf_ref[_i.a2.reg]) < (*(int32_t*)&(_i.a3.imm)) ? 1 : 0; break;
		case SLTIU: rf_ref[_i.a1.reg] = rf_ref[_i.a2.reg] < _i.a3.imm ? 1 : 0; break;
		case ANDI: rf_ref[_i.a1.reg] = rf_ref[_i.a2.reg] & _i.a3.imm; break;
		case ORI: rf_ref[_i.a1.reg] = rf_ref[_i.a2.reg] | _i.a3.imm; break;
		case XORI: rf_ref[_i.a1.reg] = rf_ref[_i.a2.reg] ^ _i.a3.imm; break;
		case SLLI: rf_ref[_i.a1.reg] = rf_ref[_i.a2.reg] << _i.a3.imm; break;
		case SRLI: rf_ref[_i.a1.reg] = rf_ref[_i.a2.reg] >> _i.a3.imm; break;
		case SRAI: rf_ref[_i.a1.reg] = (*(int32_t*)&rf_ref[_i.a2.reg]) >> _i.a3.imm; break;

		case BEQ:
			if (rf_ref[_i.a1.reg] == rf_ref[_i.a2.reg]) pc_next = _i.a3.imm;
			break;
		case BGE:
			if (*(int32_t*)&rf_ref[_i.a1.reg] >= *(int32_t*)&rf_ref[_i.a2.reg]) pc_next = _i.a3.imm;
			break;
		case BGEU:
			if (rf_ref[_i.a1.reg] >= rf_ref[_i.a2.reg]) pc_next = _i.a3.imm;
			break;
		case BLT:
			if (*(int32_t*)&rf_ref[_i.a1.reg] < *(int32_t*)&rf_ref[_i.a2.reg]) pc_next = _i.a3.imm;
			break;
		case BLTU:
			if (rf_ref[_i.a1.reg] < rf_ref[_i.a2.reg]) pc_next = _i.a3.imm;
			break;
		case BNE:
			if (rf_ref[_i.a1.reg] != rf_ref[_i.a2.reg]) pc_next = _i.a3.imm;
			break;

		case JAL:
			rf_ref[_i.a1.reg] = this->pc + 4;
			pc_next           = _i.a2.imm;
			break;
		case JALR:
			rf_ref[_i.a1.reg] = this->pc + 4;
			pc_next           = rf_ref[_i.a2.reg] + _i.a3.imm;
			break;
		case AUIPC: rf_ref[_i.a1.reg] = this->pc + (_i.a2.imm << 12); break;
		case LUI: rf_ref[_i.a1.reg] = (_i.a2.imm << 12); break;

		case HCF: nxt_state = END; break;
		case UNIMPL:
		default:
			CLASS_INFO << "Reached an unimplemented instruction!";
			if (_i.psrc) printf("Instruction: %s\n", _i.psrc);
			nxt_state = END;
			break;
	}

	if (nxt_state == PROCESS) {
		CLASS_INFO << "Instruction " << this->instrToString(_i.op)
		           << " is completed at Tick = " << acalsim::top->getGlobalTick() << " | PC = " << this->pc;
		this->pc = pc_next % MEM_BYTES;
		this->processNxtInstr();

	} else if (nxt_state == END) {
		CLASS_INFO << "Instruction " << this->instrToString(_i.op)
		           << " is completed at Tick = " << acalsim::top->getGlobalTick() << " | PC = " << this->pc;
	}
}

void CPU::memReadRespHandler(int _rd, MemReadRespPacket* _pkt) {
	CLASS_INFO << "Instruction LOAD"
	           << " is completed at Tick = " << acalsim::top->getGlobalTick() << " | PC = " << this->pc;
	uint32_t data = _pkt->getData();

	this->processInstr(_rd, data);
	this->processNxtInstr();

	acalsim::top->getRecycleContainer()->recycle(_pkt);
}
void CPU::memWriteRespHandler() {
	CLASS_INFO << "Instruction STORE"
	           << " is completed at Tick = " << acalsim::top->getGlobalTick() << " | PC = " << this->pc;
	this->processInstr();
	this->processNxtInstr();
}