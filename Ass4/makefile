test.vvp:InstructionRAM.v test_cpu.v
	iverilog -o "test.vvp" InstructionRAM.v MainMemory.v test_CPU.v CPU.v
	vvp test.vvp

.PHONY:clean

clean:
	rm test.vvp actual_memory.txt gr_1.txt