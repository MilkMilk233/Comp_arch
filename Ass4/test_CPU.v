`timescale 1ns/1ps


module CPU_test;

    //Inputs
    reg clock;
    reg start;
    integer f_addr;
    integer total_clocks;
    integer f_mem_addr;
    // integer f_gr_addr;
    integer i;

    CPU a(.clock(clock),.start(1'b0));

    initial begin
    clock=0;
    start=0;
    total_clocks = 2;

    $dumpfile("wave.vcd");        //生成的vcd文件名称
    $dumpvars(0, CPU_test);    //tb模块名称
    
    #period
    #period   

    while(a.InstrD != 32'b1 && total_clocks < 100000)
    begin
        #period 
        total_clocks = total_clocks + 1;
    end

    #period     total_clocks = total_clocks + 1;
    #period     total_clocks = total_clocks + 1;
    #period     total_clocks = total_clocks + 1;
    #period     total_clocks = total_clocks + 1;

    $display("Result memory space succesfully generated in <actual_memory.txt>");
    $display("Total clock count: %d", total_clocks);


    


    //写内存和寄存器
    f_mem_addr = $fopen("actual_memory.txt","w");
    for(i = 0; i < 512; i = i + 1)begin
        $fwrite(f_mem_addr,"%b\n",a.b.DATA_RAM[i]);
    end

    // f_gr_addr = $fopen("gr_1.txt","w");
    // for(i = 0; i < 32; i = i + 1)begin
    //     $fwrite(f_gr_addr,"%b\n",a.gr[i]);
    // end

    #period $finish;

end

parameter period=10;
always #5clock=~clock;

endmodule