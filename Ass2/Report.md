# CSC3050 Assignment2



**This project use C++ as programming language.**



###### Project component:

- simulator.cpp  
- makefile



## How to run?

Open the terminal. Place all the project component and test file in the current folder. 



##### Compile

In terminal:

Type "**make**", press "Enter"



##### Execute

In terminal:

Type "**./simulator test.asm test.txt test_checkpts.txt test.in test.out**", press "Enter"

the program will execute simulation , store the simulation result in "test.out"

Also it will produce corresponding dumped memory & registers, namely "memory_x.bin" & "register_x.bin".

*(The name of "test.asm", "test.txt" ,"test_checkpts.txt" , "test.in" and "test.out" can be changed as you need, but it should follow the order)*



##### Re-compile

In terminal:

Type "**make clean**", press "Enter"

Then follow the above "Compile" and "Execute" part.



## General Idea:

Long story short, let's divide this program in three section:

##### Section one: Memory simulation



Use malloc() to allocate space with size of 0x600000 as simulated memory space, space with size 4*35 as simulated register space (Integrated)

![Memory](https://ly-blog.oss-cn-shenzhen.aliyuncs.com/20220320_031430134_iOS.png)

##### Section two: Read instructions and data

Scan the .txt file, transform 32-bit binary instruction code into integer, place them into Text Segment (In MIPS 0x40000), initialize register all 0. Set pointers pointing to each location.



##### Section three: Simulation

Read and translate each instruction line, execute them.



## Implement details & Features

- **Use pointers to reach each section of Simulated segment**

As illustrated above, we allocate a block of memory by malloc(). Then we create pointers of vary types we need, pointing to specific locations. In this way we can reach, read and modify memory efficiently. 

- **Relation among "Relative address", "MIPS address" and "real address"**

After calling malloc(), it will return a pointer, pointing at the starting point of the newly allocated memory. So we save this two pointer, regard it as an "anchor"-That is to say, given a address in MIPS(For example 0x50000C), since we know in MIPS, the text segment starts at 0x400000, which is also where our simulated memory block starts, so we obtain its "relative address", i.e. the difference between its MIPS address and simulated starting point (which is 0x50000C - 0x400000 = 0x10000C), then we copy a "anchor" and move it forward 0x10000C units, then we reach the target address we want. In this case, 0x50000C is MIPS address, 0x10000C is relative address, and True_address + 0x10000C is real address.

- **Break codes down into functions**

To implement this, we need to use lots of duplicated codes, so we integrate them into functions, making them easy to use. For example, we store its MIPS address in registers (like 0x50000C), then we need to modify & read the content stored in PC's real memory. Therefore we need a function to transform integer, and return a pointer pointing at the memory location we want. Function get_real_address(int) work for this. This make the codes easy to understand.

- **Pointer casting**

Pointer is of different types. But in practice, we need to modify memory of different size (For example, lw needs to read a 4-byte-long word, lh needs to read a 2-bit-long halfword). We can use different types in C++ and the pointer casting to support this feature. 

- **Static data initialization**

It's not an easy task to read the content of .data section among all instruction lines. First we need to discard all the comments. Then we need to identify what the type of the content is. Since "pseudo-type" may hide in the body of the content, we need to find the type first occurred in the line. After that, we write all the bytes into the memory, fulfill the blank space with '0's.

- **Interpret instruction line and execute it**

First, we read and identify each section of the 32-bit binary code(rs, rt, rd, immediate, offset...). Then we use function str_to_reg to modify corresponding memory & register directly.

- **Syscall: Read and write**

We directly call Linux APIs open(), read() and write() to implement read and write feature of syscall. 

- **Dump memory & registers**

Just type the number in *_checkpts.txt, and the program will read from it, dump the memory and registers out for manual comparison.



## Conclusion

By doing assignment 2, I gained lots of information about memory management in C++.





###### Ref:

For LWL / SWL / LWR / SWR instruction,

[https://courses.cs.duke.edu/fall02/cps104/homework/lwswlr.html](https://courses.cs.duke.edu/fall02/cps104/homework/lwswlr.html)
