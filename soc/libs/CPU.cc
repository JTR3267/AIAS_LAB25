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

#include "AXI4Bus.hh"
#include "DataMemory.hh"
#include "event/AXI4BusAcqEvent.hh"
#include "event/ExecOneInstrEvent.hh"

CPU::CPU(std::string _name, Emulator* _emulator)
    : acalsim::SimModule(_name), pc(0), inst_cnt(0), isaEmulator(_emulator) {
	auto data_offset = acalsim::top->getParameter<int>("Emulator", "data_offset");
	this->imem       = new instr[data_offset / 4];
	for (int i = 0; i < data_offset / 4; i++) {
		this->imem[i].op      = UNIMPL;
		this->imem[i].a1.type = OPTYPE_NONE;
		this->imem[i].a2.type = OPTYPE_NONE;
		this->imem[i].a3.type = OPTYPE_NONE;
	}
	for (int i = 0; i < 32; i++) { this->rf[i] = 0; }
}

void CPU::execOneInstr() {
	// This lab models a single-CPU cycle as shown in Lab7
	// Fetch instruction
	instr i = this->fetchInstr(this->pc);
	// Execute the instruction in the same cycle
	processInstr(i);
}

void CPU::processInstr(const instr& _i) {
	bool  done   = false;
	auto& rf_ref = this->rf;
	this->incrementInstCount();
	int pc_next = this->pc + 4;

	switch (_i.op) {
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

		case LB:
		case LBU:
		case LH:
		case LHU:
		case LW:
			done = this->memRead(_i, _i.op, this->rf[_i.a2.reg] + _i.a3.imm, _i.a1);
			if (!done) return;
			break;
		case SB:
		case SH:
		case SW:
			done = !this->memWrite(_i, _i.op, this->rf[_i.a2.reg] + _i.a3.imm, this->rf[_i.a1.reg]);
			if (!done) return;
			break;

		case HCF: break;
		case UNIMPL:
		default:
			CLASS_INFO << "Reached an unimplemented instruction!";
			if (_i.psrc) printf("Instruction: %s\n", _i.psrc);
			break;
	}

	this->commitInstr(_i);
	this->pc = pc_next;
}

void CPU::commitInstr(const instr& _i) {
	CLASS_INFO << "Instruction " << this->instrToString(_i.op)
	           << " is completed at Tick = " << acalsim::top->getGlobalTick() << " | PC = " << this->pc;

	if (_i.op == HCF) return;

	// schedule the next trigger event
	auto               rc = acalsim::top->getRecycleContainer();
	ExecOneInstrEvent* event =
	    rc->acquire<ExecOneInstrEvent>(&ExecOneInstrEvent::renew, this->getInstCount() /*id*/, this);
	this->scheduleEvent(event, acalsim::top->getGlobalTick() + 1);
}

bool CPU::memRead(const instr& _i, instr_type _op, uint32_t _addr, operand _a1) {
	// If latency is larger than 1, e.g. cache miss or multi-cycle SRAM reads

	auto rc      = acalsim::top->getRecycleContainer();
	int  latency = acalsim::top->getParameter<acalsim::Tick>("SOC", "memory_read_latency");

	MemReadReqPacket* pkt = rc->acquire<MemReadReqPacket>(&MemReadReqPacket::renew, _i, _op, _addr, _a1, 1, this);

	AXI4BusAcqEvent* event = rc->acquire<AXI4BusAcqEvent>(
	    &AXI4BusAcqEvent::renew, dynamic_cast<AXI4Bus*>(this->getDownStream("DSAXI4Bus")), pkt, nullptr);
	this->scheduleEvent(event, acalsim::top->getGlobalTick() + 1);

	acalsim::top->addChromeTraceRecord(acalsim::ChromeTraceRecord::createCompleteEvent(
	    /* pid */ "CPU Read Req-" + std::to_string(pkt->getID()), /* name */ "Bus Acq Event",
	    /* ts */ acalsim::top->getGlobalTick(), /* dur */ 1, /* cat */ "", /* tid */ "",
	    /* args */ nullptr));

	return false;
}

bool CPU::memWrite(const instr& _i, instr_type _op, uint32_t _addr, uint32_t _data) {
	auto rc = acalsim::top->getRecycleContainer();

	MemWriteReqPacket* pkt   = rc->acquire<MemWriteReqPacket>(&MemWriteReqPacket::renew, _i, _op, _addr, this, 1);
	AXI4BusAcqEvent*   event = rc->acquire<AXI4BusAcqEvent>(
        &AXI4BusAcqEvent::renew, dynamic_cast<AXI4Bus*>(this->getDownStream("DSAXI4Bus")), pkt, nullptr);
	this->scheduleEvent(event, acalsim::top->getGlobalTick() + 1);

	MemWriteDataPacket* dataPkt   = rc->acquire<MemWriteDataPacket>(&MemWriteDataPacket::renew, _data, 4);
	AXI4BusAcqEvent*    dataEvent = rc->acquire<AXI4BusAcqEvent>(
        &AXI4BusAcqEvent::renew, dynamic_cast<AXI4Bus*>(this->getDownStream("DSAXI4Bus")), dataPkt, this);
	this->scheduleEvent(dataEvent, acalsim::top->getGlobalTick() + 1);

	// stall CPU pipeline
	acalsim::top->addChromeTraceRecord(acalsim::ChromeTraceRecord::createCompleteEvent(
	    /* pid */ "CPU Write Req-" + std::to_string(pkt->getID()), /* name */ "Bus Acq Event",
	    /* ts */ acalsim::top->getGlobalTick(), /* dur */ 1, /* cat */ "", /* tid */ "",
	    /* args */ nullptr));

	return false;
}

void CPU::cpuReadRespHandler(acalsim::Tick _when, MemReadRespPacket* _memReadRespPkt) {
	uint32_t data = _memReadRespPkt->getData();
	instr    i    = _memReadRespPkt->getInstr();
	operand  a1   = _memReadRespPkt->getA1();

	this->rf[a1.reg] = data;
	acalsim::top->getRecycleContainer()->recycle(_memReadRespPkt);
	commitInstr(i);
	this->pc += 4;
}

void CPU::printRegfile() const {
	std::ostringstream oss;

	oss << "Register File Snapshot:\n\n";
	for (int i = 0; i < 32; i++) {
		oss << "x" << std::setw(2) << std::setfill('0') << std::dec << i << ":0x";

		oss << std::setw(8) << std::setfill('0') << std::hex << this->rf[i] << " ";

		if ((i + 1) % 8 == 0) { oss << "\n"; }
	}

	oss << '\n';

	CLASS_INFO << oss.str();
}

instr CPU::fetchInstr(uint32_t _pc) const {
	uint32_t iid = _pc / 4;
	return this->imem[iid];
}

std::string CPU::instrToString(instr_type _op) const {
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
