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

#include "MemPacket.hh"

#include "AXI4Bus.hh"
#include "CPU.hh"
#include "DMA.hh"
#include "DataMemory.hh"

void MemReqPacket::renew(const instr& _i, instr_type _op, uint32_t _addr, acalsim::SimModule* _sender) {
	this->acalsim::SimPacket::renew();
	this->i      = _i;
	this->op     = _op;
	this->addr   = _addr;
	this->sender = _sender;
}

void MemReqPacket::visit(acalsim::Tick _when, acalsim::SimModule& _module) {
	if (auto dm = dynamic_cast<DataMemory*>(&_module)) {
		dm->memReqHandler(_when, this);
	} else if (auto axi4Bus = dynamic_cast<AXI4Bus*>(&_module)) {
		axi4Bus->axi4BusReqHandler(_when, this);
	} else if (auto dma = dynamic_cast<DMA*>(&_module)) {
		dma->dmaReqHandler(_when, this);
	} else {
		CLASS_ERROR << "Invalid module type!";
	}
}

void MemReqPacket::visit(acalsim::Tick _when, acalsim::SimBase& _simulator) {
	CLASS_ERROR << "void MemReqPacket::visit (SimBase& simulator) is not implemented yet!";
}

void MemReadRespPacket::renew(const instr& _i, instr_type _op, uint32_t _data, operand _a1,
                              acalsim::SimModule* _receiver, acalsim::SimModule* _sender) {
	this->acalsim::SimPacket::renew();
	this->i        = _i;
	this->op       = _op;
	this->data     = _data;
	this->a1       = _a1;
	this->receiver = _receiver;
	this->sender   = _sender;
}

void MemReadReqPacket::renew(const instr& _i, instr_type _op, uint32_t _addr, operand _a1, int _burstSize,
                             acalsim::SimModule* _sender) {
	this->MemReqPacket::renew(_i, _op, _addr, _sender);
	this->a1        = _a1;
	this->burstSize = _burstSize;
}

void MemWriteRespPacket::renew(const instr& _i) {
	this->acalsim::SimPacket::renew();
	this->i = _i;
}

void MemWriteReqPacket::renew(const instr& _i, instr_type _op, uint32_t _addr, uint32_t _data, int _validBytes,
                              acalsim::SimModule* _sender) {
	this->MemReqPacket::renew(_i, _op, _addr, _sender);
	this->data       = _data;
	this->validBytes = _validBytes;
}

void MemReadRespPacket::visit(acalsim::Tick _when, acalsim::SimModule& _module) {
	if (auto axi4Bus = dynamic_cast<AXI4Bus*>(&_module)) {
		axi4Bus->axi4BusReadRespHandler(_when, this);
	} else if (auto cpu = dynamic_cast<CPU*>(&_module)) {
		cpu->cpuReadRespHandler(_when, this);
	} else if (auto dma = dynamic_cast<DMA*>(&_module)) {
		dma->dmaReadRespHandler(_when, this);
	} else {
		CLASS_ERROR << "void MemReadRespPacket::visit (SimModule& module) is not implemented yet!";
	}
}

void MemReadRespPacket::visit(acalsim::Tick _when, acalsim::SimBase& _simulator) {
	CLASS_ERROR << "void MemReadRespPacket::visit (SimBase& simulator) is not implemented yet!";
}

void MemWriteRespPacket::visit(acalsim::Tick _when, acalsim::SimModule& _module) {
	CLASS_ERROR << "void MemWriteRespPacket::visit (SimModule& module) is not implemented yet!";
}

void MemWriteRespPacket::visit(acalsim::Tick _when, acalsim::SimBase& _simulator) {
	CLASS_ERROR << "void MemWriteRespPacket::visit (SimBase& simulator) is not implemented yet!";
}
