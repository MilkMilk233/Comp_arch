# CSC3050 Assignment1 

Chen Zhixin, 120090222

**This project use C++ as programming language.**



###### Project component:

- tester.cpp  (Where main function locates, be responsible  for comparison and output.)
- phase1.cpp
- phase1.h
- phase2.cpp
- makefile



## How to run?



Open the terminal. Place all the project component in the current folder. 



##### Compile

In terminal:

Type "**make**", press "Enter"



##### Execute

In terminal:

Type "**./tester input.asm output.txt expectedoutput.txt**", press "Enter"

the program will execute assemble jobs, stored the binary codes in the "output.txt".

Also, it will automatically compare "output.txt" and "expectedoutput.txt" as you provided. 

*(The name of "input.asm", "output.txt" and "expectedoutput.txt" can be changed as you need, but it should follow the order)*



##### Re-compile

In terminal:

Type "**make clean**", press "Enter"

Then follow the above "Compile" and "Execute" part.





## General Idea

The program can be divided into **phase 1** and **phase 2**.



##### Phase 1:

Scan through all the lines, discard all the useless parts, such as comments and .data section. 

Meanwhile use a map to record the address of the label, which plays the role of "**labelTable**".



##### Phase 2:

Eliminate all the blank space (For robustness purpose).

Detect the instruction type.

Combined with the stored label address, translate the instruction into binary code.





## Implementation details & Features



- **Using map in STL to store the relationship between labels and their "address"**

  The mapping is unique, say that each label correspond to only one "address", that is the actual address of the next valid instruction. So in the program, we use a STL map to record this kind of relationship. 

  

- **The .data and .text section are allowed to appear repeatedly and alternately**

  In this program, we use a Boolean type "isData" to tell whether the line pc pointing to is in the .data section. When first encounter .data, the "isData" will be set to "false", disable the for loop until we meet the .text section. Once pc encounter .text, the "isData" will be set to "true" and the next few steps are to be carried out.

  

- **The comments will be eliminated no matter where they are**

  The program will scan the line to see whether "#" exists. If so, everything following will be ignored.

  

- **Eliminate all the blank space & tab to support "free format" feature of assembler**

  To make the program robust enough, we allow users to place any blank space between each section of the instruction. That requires us to handle these blank spaces when proceeding these instruction lines. So in this program, all the blank space and tabs are ignored. We use the remaining part to identify each keyword.

  

  

  

- **Address system**

  Label needs locations to tell where they are pointing at. Say that the address of the instructions starts from 0x400000 in this project. When switch to the next instruction line, the actual address increase by 4. Here in this project, we use "relative address", say this is the No. of each instruction line. The first instruction, which has the actual address of 0x400000, has "relative address" 0. The second instruction line, which has the actual address of 0x400004, has "relative address" 1. And so on. Later we will discuss more about it.

  

- **Ways to locate registers and replace with corresponding binary digits**

  We use a const map in phase2.cpp to store information about registers. Since the relationship between register name and their binary code are fixed, every time we need to fulfill space with register binary code, we would call the const map stored in phase2.cpp to transfer their name to binary code.

  Since all the registers has a prefix "$", what we need to do is just to find all the "$" and read the following 2 bits, which is enough for us to distinguish what this type of register is.

  

- **Labels: Identification, binary calculation**

  Labels only appear in following kind of instructions:

  |   beq    |  bgez   | bgtz  |  blez   |
  | :------: | :-----: | :---: | :-----: |
  | **bltz** | **bne** | **j** | **jal** |

  Also, they only appear at the end of the instruction line.

  We consider following two cases:

  1. J or JAL: According to the type of instruction line, read following digits except the first 1 (3) digits. Then we obtain the name of the label, since all the spaces and tabs are already discarded.

  2. Rest: find where the last "," is. The digits between the last "," and the end of the line is exactly what the name of the label is.

     

- **Immediate: 2's complement, decimal to binary and sign-extent**

  The immediate (also the shift amount "sa") we read from the instruction line is in decimal form, also string in C++ STL type. Thus, we need to transform it into binary form with string type. 

  First we identify whether it is negative. If so, discard the negative sign and record it.

  After that, we transform it into integer in C++ STL type. 

  Then we use algorithm-divide and remainder, to transform it into binary form. Here we use a stringstream type to catch all the remainders, string them together and use the .str() method convert them into string type.

  If it's negative, we need to carry out 2's complement. We scan the binary code from LSB to MSB. After encounter the first "1", we invert every bit then, say from "1" to "0", and from "0" to "1".

  What's more, we carry out sign-extent on demand. The I-Type needs a 16-bit-long binary string, while J-Type needs 25. So we extent the binary string, add multiple "1"s to negative number of multiple "0"s to positive numbers, until we meet the demand.

  

- **To transfer data between functions: Using "struct" to pass multiple parameters**

  Unlike python, in C++, we cannot return multiple values. So we use a struct as a carrier of both the labelTable "map" type and the vector with all the lines to transfer data between functions.

   

- **Distinguish each kind of operation, instruction**

  Except for j and jal type, all the instruction line's second "token" represents a register. Also, at the beginning of the register, there is a unique "$". We make use of this feature, knowing that the bits between the beginning of the instruction line and the "$" sign is exactly what we want-since all the blank space and tabs has been discarded.

  As for the j and jal type, first we search if this line is above kind of instructions. If not, then we can confirm it is one of the two types in j and jal. We use else if structure, first determine if it's jal type. If not, it's j type.

  

- **Offset & Address in I-Type and J-Type**

  Labels appear in I-Type in offset form, which measures the distance between the target and current pc position. While labels appear in I-Type in address, which maps the actual address by shifting left for 2 bits, i.e., multiple by 4. We use target label"relative address" minus current for loop counter to get what should be placed in "offset", and use instruction divided by 4 adding "relative address" to get what should be placed in "address". In this scenario, it's far more convenient to use "relative address" as illustrated above, to record the position of each label. 



## Conclusion

By doing this project, I learn & review a lot of C++ grammar. Also, I have a deeper understanding of how assembler of MIPS works.
