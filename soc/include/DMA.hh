#ifndef SOC_INCLUDE_DMA_HH_
#define SOC_INCLUDE_DMA_HH_

#include <queue>
#include <string>

#include "ACALSim.hh"
#include "MemPacket.hh"

typedef struct DMAInfo {
	MemReadReqPacket* memReadReqPkt;
	int               bytes;
	int               burstSize;
	uint32_t          destAddr;
} DMAInfo;

class DMA : public acalsim::SimModule {
public:
	DMA(std::string _name, int _base_addr, int _size);

	virtual ~DMA() {}

	void dmaReqHandler(acalsim::Tick _when, MemReqPacket* _memReqPkt);

	void triggerNextReq();

	void dmaReadReqHandler(MemReadReqPacket* _memReadReqPkt);

	void dmaWriteReqHandler(MemWriteReqPacket* _memWriteReqPkt);

	void dmaReadRespHandler(acalsim::Tick _when, MemReadRespPacket* _memReadRespPkt);

	void dmaStart();

	void triggerNextDMA(int _latency);

	void dmaEnd();

	const int getBaseAddr() { return this->base_addr; };

private:
	int                       base_addr;
	int                       size;
	uint32_t                  rf[6];
	std::queue<MemReqPacket*> pending_req_queue;
	std::queue<DMAInfo>       dma_info_queue;
	bool                      is_idle = true;
	uint32_t                  curDestAddr;
	int                       curBytes;
	int                       curBurstCount;
	int                       curBurstSize;
};

#endif
