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

#include "DataMemory.hh"

using namespace acalsim;

void MemReadReqPacket::visit(Tick when, SimModule& module) {
	if (dynamic_cast<DataMemory*>((SimModule*)(&module))) {
		dynamic_cast<DataMemory*>((SimModule*)(&module))->memReadReqHandler(when, this);
	} else {
		CLASS_ERROR << "Invalid module type!";
	}
}

void MemReadReqPacket::visit(Tick when, SimBase& simulator) {
	CLASS_ERROR << "void MemReadReqPacket::visit (SimBase& simulator) is not implemented yet!";
}

void MemWriteReqPacket::visit(Tick when, SimModule& module) {
	if (dynamic_cast<DataMemory*>((SimModule*)(&module))) {
		dynamic_cast<DataMemory*>((SimModule*)(&module))->memWriteReqHandler(when, this);
	} else {
		CLASS_ERROR << "Invalid module type!";
	}
}

void MemWriteReqPacket::visit(Tick when, SimBase& simulator) {
	CLASS_ERROR << "void MemWriteReqPacket::visit (SimBase& simulator) is not implemented yet!";
}

void MemReadRespPacket::visit(Tick when, SimModule& module) {
	CLASS_ERROR << "void MemReadRespPacket::visit (SimModule& module) is not implemented yet!";
}

void MemReadRespPacket::visit(Tick when, SimBase& simulator) {
	CLASS_ERROR << "void MemReadRespPacket::visit (SimBase& simulator) is not implemented yet!";
}