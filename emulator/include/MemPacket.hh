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

#ifndef EMULATOR_INCLUDE_MEMPACKET_HH_
#define EMULATOR_INCLUDE_MEMPACKET_HH_

#include <cstdint>
#include <functional>

#include "ACALSim.hh"
#include "DataStruct.hh"

class MemReadRespPacket;

class MemReadReqPacket : public acalsim::SimPacket {
public:
	MemReadReqPacket() {}
	MemReadReqPacket(std::function<void(MemReadRespPacket*)> _callback, instr_type _op, uint32_t _addr)
	    : acalsim::SimPacket(), callback(_callback), op(_op), addr(_addr) {}
	virtual ~MemReadReqPacket() {}

	void renew(std::function<void(MemReadRespPacket*)> _callback, instr_type _op, uint32_t _addr);

	void visit(acalsim::Tick _when, acalsim::SimModule& _module) override;
	void visit(acalsim::Tick _when, acalsim::SimBase& _simulator) override;

	const instr_type& getOP() { return this->op; }
	const uint32_t&   getAddr() { return this->addr; }
	auto              getCallback() { return this->callback; }

private:
	instr_type op;
	uint32_t   addr;

	std::function<void(MemReadRespPacket*)> callback;
};

class MemWriteReqPacket : public acalsim::SimPacket {
public:
	MemWriteReqPacket() {}
	MemWriteReqPacket(std::function<void(void)> _callback, instr_type _op, uint32_t _addr, uint32_t _data = 0)
	    : acalsim::SimPacket(), callback(_callback), op(_op), addr(_addr), data(_data) {}
	virtual ~MemWriteReqPacket() {}

	void renew(std::function<void(void)> _callback, instr_type _op, uint32_t _addr, uint32_t _data);

	void visit(acalsim::Tick _when, acalsim::SimModule& _module) override;
	void visit(acalsim::Tick _when, acalsim::SimBase& _simulator) override;

	const instr_type& getOP() { return this->op; }
	const uint32_t&   getAddr() { return this->addr; }
	const uint32_t&   getData() { return this->data; }
	auto              getCallback() { return this->callback; }

private:
	instr_type                op;
	uint32_t                  addr;
	uint32_t                  data;
	std::function<void(void)> callback;
};

class MemReadRespPacket : public acalsim::SimPacket {
public:
	MemReadRespPacket() {}
	MemReadRespPacket(uint32_t _data) : acalsim::SimPacket(), data(_data) {}
	virtual ~MemReadRespPacket() {}

	const uint32_t& getData() { return this->data; }

	void renew(uint32_t _data);

	void visit(acalsim::Tick _when, acalsim::SimModule& _module) override;
	void visit(acalsim::Tick _when, acalsim::SimBase& _simulator) override;

private:
	uint32_t data;
};

#endif
