# CSC3050 Assignment 4 Report

[TOC]

## Overview

In this project, I implemented a 5-stage pipelined CPU with Verilog, for each stage responsible for one different functionality. It can handle jump / branch instructions, and relevant data hazards. The first stage is Instruction fetch from (instruction) memory , **or "IF" for short.** The second is Instruction decode & register read, **or "ID" for short.**  The third is Execute operation or calculate address, **or "EX" for short.** The fourth is Access (data) memory operand, **or "MEM" for short.** The fifth is Write the result back to register, **or "WB" for short.** 

We break down executing every MIPS instruction into five stages. Since there is no need to wait until previous instruction completed, the process is significantly accelerated.

<img src="https://ly-blog.oss-cn-shenzhen.aliyuncs.com/CSC3050/illustration.png" alt="Illustration" style="zoom: 33%;" />

The 5-stage pipelined is encapsulated in one Verilog module named **CPU** , it has two inputs: `clock` for outer driven clocks, `start` for initialization.

> Component: CPU.v ; test_CPU.v ; InstructionRAM.v ; Mainmemory.v ; makefile ; 
>
> Ways of operation: Change the name of your input MIPS Machine code file to `instruction.bin`, put it under the current folder, then `make`. It will generate output file `actual_memory.txt`, representing memory content after execution.

## Data path

![Datapath](https://ly-blog.oss-cn-shenzhen.aliyuncs.com/CSC3050/IMG_3209.JPG)



The design of this diagram is based on task description. Based on that, there is more hardware connections to support jump instructions, also to handle various data hazards.



## Five Stages

### Instruction fetch

In this stage, the instruction will be extracted from target address from Instruction memory module. The module is encapsulated in `InstructionRAM.v`, imported by `CPU.v` as object `a`. On every positive edge of the clock, It will output target instruction `RD` based on current address `PCF`. The address vary, depending on previous instruction type: jump, branch or others.

### Instruction decoding

In this stage, the instruction will break down into several fragments. For the analysis propose, the Control Unit will  output different signals determining what circuits need to do next. 

Given the address of the register, the general register will fetch the content on the **negative edge** of the clock. The reason why is to support reading and writing simultaneously, which we will discuss later.

Also, some important fragments are collected and passed into next stage(maybe with some small modifications).

### Execution

In this stage, ALU will calculate the result, based on the calculation type determined by "ID" stage. Also, it is closely connected to `Forwarding Unit`, which detects data hazards and respond.

ALU will also do some comprehensive analysis on some specific instructions(like `beq`, which will output signal `Zero`).

### Memory Management

Sometimes, circuit need to write/read data from memory. In this stage, the circuit is connected to Main memory module, which is encapsulated in `Mainmemory.v`. At every positive edge of the clock, the module will be triggered, and the data can be modified or read via `WriteDataW` and `RD`.  Also, in this stage, the circuit will handle the calculation result of ALU, and decide whether to jump the program counter.

### Write back

In this stage, the circuit will decide whether to write data back to the registers. Of course it is optional, all depends on whether the signal `MemtoRegW` and `RegWriteW` is activated or not.



## Hazards

### MEM_to_EX / WB_to_EX hazard

Sometimes, there will be cases like:

```verilog
add x1,x1,x2
add x1,x1,x3
add x1,x1,x4
```

In original design, the data will be update in "WB" stage. But now the instructions crowds in one after another. For this scenario, we can take two different strategies:

1. Wait until the last conflicted instruction completely done. 		--Insert bubbles.
2. Get information in advance        --Forwarding

In this circuit, we decide to use strategy #2. This will take more circuit connections, but more efficiency in operation. To be specific, we follows the design principle as below:

**EX hazard** (Result of Instruction in EX occurs in source of Instruction in ID)

> if (EX/MEM.RegWrite and (EX/MEM.RegRd != 0) and (EX/MEM.RegRd=ID/EX.RegRs))
>
> ForwardA=10



**MEM hazard** (Result of Instruction in MEM occurs in source of Instruction in ID)

> if (MEM/WB.RegWrite and (MEM/WB.RegisterRd != 0) and not(EX/MEM.RegWrite and (EX/MEM.RegisterRd != 0) and (EX/MEM.RegisterRd = ID/EX.RegisterRs1))
>
>  and (MEM/WB.RegisterRd = ID/EX.RegisterRs1)) 
>
> ForwardA = 01



Also, when two types of hazards occurs at the same time, we need the most recent one, that is to handle EX hazard at first priority.



### Stalls

Sometimes forwarding strategy does not fit all types of data hazards. For example, when executing `lw` instruction, we need to Fetch data from "MEM" stage. But if the following instruction need to use `lw`'s result as the source, then forwarding fails to work. Instead, we need some "backward", that is to create "Stalls". We should let the following instruction above wait for 1 cycle when reaching "ID" stage. Then, at next cycle, the previous `lw` instruction reach "MEM", successfully load data from Main memory module, then we can activate "Forward Unit" to supply following instructions with fresh fetched data. The key is to identify "Stalls", by recognizing `lw` instructions and compare between its destination register and the following source of the instruction.



### Jump and branch 

Here we discuss them together, because they share almost the same way of operation, except some minor difference. Once detects `j`, `jr`, `jal`, `beq`, `bne` instructions in "EX" stage, we need to eliminate following several instructions, as if they have never appears. This can be easily done, because they haven't modify data in Registers or Memories, there is no need to go deeper, what we only need is to set the value to 0 when transferring data among "IF", "ID", "EX" and "MEM". Then, we modify `PCBranchM`, and jump there with the signal `PCSrcM` set to 1. for instructions `jal`, we just need to let the stages "WB" carry out normally, then we can modify the corresponding registers. The difference between `jr` and `j` is that, the source of data are different, one is from 26-bit-long offset, the other is from address stored in specific registers.



## Performance on test cases

AC: 8/8

Testcase1: Total clock count = 58

Testcase2: Total clock count = 17

Testcase3: Total clock count = 20

Testcase4: Total clock count = 19

Testcase5: Total clock count = 241

Testcase6: Total clock count = 66

Testcase7: Total clock count = 51

Testcase8: Total clock count = 33



## Conclusion

This project really helps me to understand further about computer architecture, also strengthen my understanding in Verilog language. I found it very useful on my career / study life. 

 
