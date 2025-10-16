## HW 25-1 AXI4 Bus Modeling

Please provide the commands and all necessary resources for us to reproduce your results. 
```shell
$ BUILD_DIR="build/debug" && \
    cmake -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Debug && \
    cmake --build "${BUILD_DIR}" -j $(nproc) --target soc
$ ./build/debug/bin/soc --asm_file_path ./soc/asm/target_file
```

1. Please create a timing diagram illustrating how you plan to model the latencies introduced by the AXI4 bus during CPU load and store instructions. You may refer to the schematic diagrams shown in [Lab 25-2](https://course.playlab.tw/md/g7aTCHr1TvquhvtDhUupFQ#End-to-end-Timeline-of-the-Demo-Example) as an example. 
    
    - `Load`
    
    ![](https://course.playlab.tw/md/uploads/b741f9c7-83c5-4b7f-9212-62bfd070158d.png)
    
    - `Store`
    
    ![](https://course.playlab.tw/md/uploads/ed537516-8600-4c97-9536-0fb0a09018e2.png)

3. Please use Chrome Tracing to visualize the stages and latencies involved in bus read and write transactions during simulation. Ensure your screenshots clearly demonstrate that the timing behavior matches the definition in the homework requirements.

![](https://course.playlab.tw/md/uploads/5b21307c-10ee-43a9-8189-4bf2c2fbe878.png)

3. Provide screenshots showing the final register file contents and simulated clock cycles for the following test cases.
    - `soc/asm/load_store_simple.txt`

    ![](https://course.playlab.tw/md/uploads/0a167212-4296-4df5-b7e6-19dbc59961c1.png)
    
    - `soc/asm/full_test.txt`

    ![](https://course.playlab.tw/md/uploads/cd70526b-4f1c-484f-a32e-41b2f58b8388.png)

    - The assembly program provided in the HW 25-1 description.
    
    ![](https://course.playlab.tw/md/uploads/fdf5d6d2-f632-426b-b1b5-a96773d204fe.png)

## HW 25-2 DMA Modeling

Please provide the commands and all necessary resources for us to reproduce your results. 
```shell
$ BUILD_DIR="build/debug" && \
    cmake -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Debug && \
    cmake --build "${BUILD_DIR}" -j $(nproc) --target soc
$ ./build/debug/bin/soc --asm_file_path ./soc/asm/target_file
```
修改 soc/configs.json 中 `"bus_burst_mode": "FIXED"` 對應到 Burst Mode 1 `"bus_burst_mode": "INCR"` 對應到 Burst Mode 2

1. Please draw a timing diagram explaining the latencies associated with a DMA command. The MMIO read/write operations performed by the CPU before and after DMA processing should be included.
    - `Burst Mode 1`
    假設 DMA 只需要搬移 4 bytes 資料 (CPU 讀取 MMIO register 的路徑與寫入 MMIO register 相同，MMIO register 的 read response 路徑與 data memory read response 相同，故省略)
    
    ![](https://course.playlab.tw/md/uploads/e958dcac-b0ec-40cb-9787-b6b4cff9644b.png)
    
2. Demonstrate through visualization that your simulator's timing model aligns with the expected behavior described in the previous point.
    
    Burst Mode 2 的模擬圖，在 CPU program MMIO register 的下個 cycle，DMA 發出 burst length 4 的 read request，並在收到 4 筆 response 後的下個 cycle 送出 burst length 4 的 write request
    
    ![](https://course.playlab.tw/md/uploads/feab16bb-3301-4675-ad89-2135b430a556.png)

3. Use simulation logs or Chrome Tracing to verify that your AXI bus model (from HW 25-1) correctly simulates burst modes with lengths of 1, 2, and 4 during DMA transactions.
    - `Burst Mode 1`
    DMA 一次發出一個 read request，在收到 read response 的下個 cycle 發出 write request，再下個 cycle 再送出 read request

    ![](https://course.playlab.tw/md/uploads/1e7ffaaf-476b-451b-aef5-01ebed116c23.png)

    - `Burst Mode 2`
    DMA 自動選擇適合的 burst length (4)，發出一筆 burst length 4 的 read request 後，在收到所有 read response 的下個 cycle 送出 burst length 4 的 write request，在 4 筆 write request 都送出後的下個 cycle 送出下個 burst length 4 的 read request
    
    ![](https://course.playlab.tw/md/uploads/a16f1fdb-e205-4ba7-a7e0-cfd9de68f130.png)

4. Execute the provided assembly program (in HW 25-2 description) and attach the screenshots of the final register file contents and simulated clock cycles to confirm the correct functionality of your DMA model.
    - `Burst Mode 1`
    
    ![](https://course.playlab.tw/md/uploads/955b3006-66d8-4416-84dc-34193d115013.png)
    
    - `Burst Mode 2`
    
    ![](https://course.playlab.tw/md/uploads/06087d11-48b7-4207-af95-7ff68bdad580.png)


## HW 25-3 End-To-End Integration and Testing

Please provide the commands and all necessary resources for us to reproduce your results. 
```shell
$ BUILD_DIR="build/debug" && \
    cmake -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Debug && \
    cmake --build "${BUILD_DIR}" -j $(nproc) --target soc
$ ./build/debug/bin/soc --asm_file_path ./soc/asm/hw3.txt
```
修改 soc/configs.json 中 `"bus_burst_mode": "FIXED"` 對應到 Burst Mode 1 `"bus_burst_mode": "INCR"` 對應到 Burst Mode 2

1. Please paste your testbench assembly in this section

```asm
# [TODO] paste your code here
.text
start:
    li   x1, 0x2000
    li   x2, 0x2fff

initialize_loop:
    slli x3, x1, 1
    sw   x3, 0(x1)

    addi x1, x1, 4
    blt  x1, x2, initialize_loop

program_dma_1:
    li   x1, 0xf000
    li   x2, 0x2000
    li   x3, 0x3000
    li   x4, 0x80807f1f
    li   x5, 1

    # Program DMA
    sw   x2, 4(x1)
    sw   x3, 8(x1)
    sw   x4, 12(x1)
    sb   x5, 0(x1)

check_complete_1:
    # Check whether the DMA command completes
    lw   x6, 20(x1)
    and  x6, x6, x5
    beq  x6, x0, check_complete_1
    sw   x0, 20(x1)

program_dma_2:
    li   x2, 0x2e00
    li   x3, 0x4000
    li   x4, 0x80807f0f

    # Program DMA
    sw   x2, 4(x1)
    sw   x3, 8(x1)
    sw   x4, 12(x1)
    sb   x5, 0(x1)

check_complete_2:
    # Check whether the DMA command completes
    lw   x6, 20(x1)
    and  x6, x6, x5
    beq  x6, x0, check_complete_2
    sw   x0, 20(x1)

exit:
    hcf

```

2. Please provide the screenshots of simulator execution logs 
    - `Burst Mode 1`
    
    ![](https://course.playlab.tw/md/uploads/35054660-29ae-46b6-b240-3988153c5cea.png)

    - `Burst Mode 2`

    ![](https://course.playlab.tw/md/uploads/ab8b8ee0-d187-4886-8b93-5fe4a84146d4.png)

3. Please paste the memory contents to validate the correctness of your integrated simulator. 
https://course.playlab.tw/git/JTR3267/lab25/-/blob/hw3/memdump.txt

## HW 25-4 Performance Analysis and Comparison

Please provide the commands and all necessary resources for us to reproduce your results. 
```shell
$ BUILD_DIR="build/debug" && \
    cmake -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Debug && \
    cmake --build "${BUILD_DIR}" -j $(nproc) --target soc
$ ./build/debug/bin/soc --asm_file_path ./soc/asm/hw3.txt
```
修改 soc/configs.json 中 `"bus_burst_mode": "FIXED"` 對應到 Burst Mode 1 `"bus_burst_mode": "INCR"` 對應到 Burst Mode 2

Compare the performance of your testbench from HW 25-3 using different AXI burst modes. Analyze the results and discuss whether the observed performance meets your target goals.

在 Burst Mode 1 中如果要讀寫 16 bytes 資料需要 4 * 10 = 40 cycles (假設 mem read 需要 5 cycles)
cycle 1: dma -> bus (read)
cycle 2: bus -> mem (read)
cycle 3 ~ 7: mem read (read)
cycle 8: mem -> bus (read)
cycle 9: bus -> dma (read)
cycle 10: dma -> bus (write)

在 Burst Mode 2 中如果要讀寫 16 bytes 資料需要 28 cycles
cycle 1: dma -> bus
cycle 2: bus -> mem
cycle 3 ~ 7: burst1 mem read
cycle 8: burst1 mem -> bus & burst2 mem read
cycle 9: burst1 bus -> dma & burst2 mem read
cycle 10 ~ 12: burst2 mem read
cycle 13 ~ 17: burst3 mem read
cycle 18 ~ 22: burst4 mem read
cycle 23: burst4 mem -> bus
cycle 24: burst4 bus -> dma
cycle 25: burst length 4 mem write req & burst1 mem write data dma -> bus
cycle 26: burst2 mem write data dma -> bus
cycle 27: burst3 mem write data dma -> bus
cycle 28: burst4 mem write data dma -> bus
(中間省略 burst2, burst3 data 回傳)

由上述範例可見 Burst Mode 2 在 16 bytes 讀寫中花費的 cycle 數比 Burst Mode 1 少 12 個 cycle，在 hw3 的部分，總共有 (4096 + 2048) / 16 = 384 次 16 bytes 讀寫，預計 Burst Mode 2 可以比 Burst Mode 1 少 384 * 12 = 4608 個 cycle，與結果相符(19522 - 14914 = 4608)
