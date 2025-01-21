// demo/main.cc

#include <string>

#include "ACALSim.hh"
#include "Memory.hh"
#include "TrafficGenerator.hh"

class ExampleSimulator : public acalsim::STSimBase {
public:
	ExampleSimulator(std::string _name = "ExampleSimulator") : acalsim::STSimBase(_name) {}

	void registerModules() override {
		TrafficGenerator* tg  = new TrafficGenerator();
		Memory*           mem = new Memory();

		tg->addDownStream(mem, "DSMem");
		mem->addUpStream(tg, "USTrafficGenerator");

		this->addModule(tg);
		this->addModule(mem);
	}

	void simInit() override { LABELED_INFO(this->getName()) << "ExampleSimulator::simInit() is invoked."; }
	void step() override { LABELED_INFO(this->getName()) << "ExampleSimulator::step() is invoked."; }
	void cleanup() override { LABELED_INFO(this->getName()) << "ExampleSimulator::cleanup() is invoked."; }
};

int main(int argc, char** argv) {
	acalsim::top = std::make_shared<acalsim::STSim<ExampleSimulator>>();
	acalsim::top->init(argc, argv);
	acalsim::top->run();
	acalsim::top->finish();
	return 0;
}
