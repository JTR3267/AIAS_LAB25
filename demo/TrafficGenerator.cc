// demo/TrafficGeneraotr.cc

#include "TrafficGenerator.hh"

TrafficGenerator::TrafficGenerator(std::string _name) : acalsim::SimModule(_name) {
	LABELED_INFO(this->getName()) << "Constructing...";
}

std::vector<RequestInfo> TrafficGenerator::genRequests() const {
	auto req_vec = std::vector<RequestInfo>();

	for (size_t i = 0; i < 4; ++i) {
		req_vec.emplace_back(RequestInfo{.issue_tick_ = i, .addr_ = 0x400, .size_ = ((uint32_t)i + 1) * 2});
	}

	return req_vec;
}

bool TrafficGenerator::isAllReqCompleted() const { return false; }
