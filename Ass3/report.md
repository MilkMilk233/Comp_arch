# CSC3050 Assignment3 Report

This project use Verilog as programming language.



###### Project component:

- `ALU.v`
- `test_ALU.v`
- `makefile`



## How to run?

```bash
make
```

*(Use* `make clean` *to initialize)*

## General Idea:

#### Big picture

Arithmetic and Logic Unit, or ALU for short, is the execution unit of CPU. It performs binary arithmetic calculation, including add, subtract, bits shifting, etc. Given inputs(registers in memory), it performs calculation on them and gives the result.

##### Block diagram:

![Block_diagram](https://ly-blog.oss-cn-shenzhen.aliyuncs.com/%E5%BE%AE%E4%BF%A1%E5%9B%BE%E7%89%87_20220409121402.jpg)

##### Simple Register Fetch

Since we don't simulate the allocation of the registers in this assignment, we use regA and regB to simulate the target memory registers in real situations. The address of regA is `00000`, while the address of regB is `00001`. When we scan the MIPS instruction, we will refer to regA or regB according to the codes fragment of rt/rs/rd. For example, instruction[25:21] is the address of rs, in an instance it is `00000`, then we regard regA as the source of rs. Then we can extract the correct content of each registers and send them into ALU core part for further process. 

##### ALU

In this program, the core part of ALU are integrated into "case" branches. First it scan the opcode and func to determine which kind of instruction it is. Then, it process the fetched registers and output the results and flags.

## Implement details & Features

-  `reg` **in Verilog**

`reg` stands for registers. It can store value (binary digits) in it. Also, we can extract data in it if needed. In this program, we define these `reg`:

1. `opcode`, `func`, `rs`, `rt`, `rd`: Extract fragments in MIPS instruction and store them in these registers.

2. `reg_A`, `reg_B`, `reg_C`: temporary storage for ALU inputs & output.

3. `unsigned_reg_A`,`unsigned_reg_B`,`unsigned_reg_C`: Like "pointer casting" in C++, we use this to convert digits into unsigned form, to feed the needs of some instructions like `addu`,`subu` , etc.

4. `extension`: Temporary storage of judging `overflow`

5. `zero`,`negative`, `overflow`: Record of each digits in `flag`

   

- `signed` **in Verilog**

As illustrated above, we use `signed` prefix to declare a register, then when we perform binary calculation, it can be regarded as signed numbers, which can simplify some process like `slt` etc.



- `case` **in Verilog**

`case` in Verilog can make program get into different branches according to vary conditions, similar to the if/ else if structure in C++, slightly different from case structure in C++. In this assignment, first the opcode is detected. According to the opcode, it decides to go to branch of I-type or R-type. Then, if it's R-type, it go to deeper branches according to the func fragment code. So far, the type of MIPS instruction is determined.



- **How to detect overflow**

We need to handle with overflow problems when processing `add`, `addi`, `subu`. To detect overflow, we use 33-bit addition/subtraction. First we sign-extent two 32-bit adder to 33 bits. If it's original negative, then we add a 1 on the MSB. Otherwise we add zero. Then we perform natural addition. The result will get an 33-bit binary digits. Then we using XOR to compare the MSB and 2nd MSB in the result. If overflow occurs, the result of XOR will be 1. Otherwise, the result of XOR will be 0. For Example, `0100...001` (32 bits)+ `0100...001`(bits), assume the result will overflow. Then, after addiction, the result is `1000...010.` If we use sign-extent, the addition becomes `00100...001 `+ `00100...001 ` = `01000...010`, The MSB XOR 2nd MSB will be 1, which correspond to our algorithm. The other examples are similar. In general, we use sign-extent conversion to preserve sign digits(when two adder's sign are the same) and check if it's changed after addition.



- **How to detect zero**

Direly compare the outcome with 0. If equal, then assign output "zero" to `1`.



- **How to detect negative**

Check the MSB of outcome. If MSB is `1`, then assign output "negative" to `1`.



- **Arithmetic vs. Logical shifting**

It's not an easy task to implement arithmetic and logical shifting. However, thanks to the grammar of Verilog, the bit calculation `>>` and `>>>` directly supports the logical and arithmetic shifting. The former is for logical shifting, while the latter is for arithmetic shifting.



- **Register fetch**

Since we don't simulate the allocation of the registers in this assignment, we use regA and regB to simulate the target memory registers in real situations. The address of regA is `00000`, while the address of regB is `00001`. When we scan the MIPS instruction, we will refer to regA or regB according to the codes fragment of rt/rs/rd. For example, instruction[25:21] is the address of rs, in an instance it is `00000`, then we regard regA as the source of rs. Then we can extract the correct content of each registers and send them into ALU core part for further process. 



- **Immediate: sign-extent or zero-extent?**

Except for addi, ori, xori, which uses zero-extent strategy, any other instructions involving immediate uses sign-extent strategy.



## Conclusion

In this assignment, I learned fundamental usage of Verilog, meanwhile know more about ALU.
