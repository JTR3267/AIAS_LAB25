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

/* --------------------------------------------------------------------------------------
 *  A test template to demonstrate how to create your own single-thread simulator using this framework
 *  Step 1. Inherit SimModule to create your own module class
 *  Step 2. Inherit STSimBase to create your own simulator class
 *  Step 3. instantiate a top-level simulation instance and call the following APIs in turn
 *          1) STSim::init(); //Pre-Simulation Initialization
 *          2) STSim::run();  //Simulation main loop
 *          3) STSim::finish(); // Post-Simulation cleanup
 * --------------------------------------------------------------------------------------*/

#include "ACALSim.hh"
using namespace acalsim;

#include "SystemTop.hh"

int main(int argc, char** argv) {
	// Step 3. instantiate a top-level simulation instance
	// Remember 1) to cast the top-level instance to the SimTop* type and set it to the global variable top
	// 2) Pass your own simulator class type to the STSim class template
	top = std::make_shared<SystemTop>("SystemSim", "emulator/configs.json");
	top->init(argc, argv);
	top->run();
	top->finish();
	return 0;
}
