`timescale 1ns / 1ps
module CPU(clock, start);
    input clock;
    input start;

    reg [31:0] PCF;      //Program Counter
    reg [31:0] PCPlus4F;
    reg [31:0] PCPlus4D;
    reg [31:0] PCPlus4E;
    reg [31:0] PCbar;      //Program Counter ' 
    reg [31:0] gr[31:0];  //General Register，即cycle 2 中的内容，存放在CPU.v内
    reg [31:0] RD;      // Cycle 1 中根据PC抓取的目标instruction content
    reg [31:0] InstrD;   // Cycle 2 中被分解的instruction content, 和RD内容一致
    reg [31:0] ALUOutE;
    reg [31:0] ALUOutM;
    reg [31:0] ALUOutW;
    reg [31:0] ReadDataW;
    reg [31:0] ResultW;
    reg [31:0] WD3;
    reg [31:0] RD1;        
    reg [31:0] RD2D;

    reg [31:0] RD2E;
    reg [31:0] SrcAE;
    reg [31:0] SrcBE;
    reg signed [31:0] sign_SrcAE;
    reg signed [31:0] sign_SrcBE;
    reg [31:0] PCBranchE;
    reg [31:0] PCBranchM;
    reg [31:0] WriteDataE;
    reg [31:0] WriteDataM;
    reg [31:0] WriteDataW;
    reg [31:0] SignImmD;
    reg [31:0] SignImmE;
    reg [31:0] ShiftImm;
    reg [5:0] Op;
    reg [5:0] Funct;
    reg [4:0] A1;
    reg [4:0] A2;
    reg [4:0] A3;
    reg [4:0] ALUControlD;
    reg [4:0] ALUControlE;
    reg [4:0] shamtD;       // Shift amount要单独判断
    reg [4:0] shamtE;
    reg [4:0] RtE;
    reg [4:0] RdE;
    reg [4:0] WriteRegE;
    reg [4:0] WriteRegM;
    reg [4:0] WriteRegW;
    reg ALUSrcD;
    reg ALUSrcE;
    reg extension;
    reg SaCtrlD;
    reg SaCtrlE;
    reg overflow;
    reg MemWriteD;
    reg MemWriteE;
    reg MemWriteM;
    reg MemtoRegD;
    reg MemtoRegE;
    reg MemtoRegM;
    reg MemtoRegW;
    reg RegWriteD;
    reg RegWriteE;
    reg RegWriteM;
    reg RegWriteW;
    reg RegDstD;
    reg RegDstE;
    reg BranchD;
    reg BranchE;
    reg BranchM;
    reg PCSrcM;
    reg ZeroE;
    reg ZeroM;
    reg Mem_clock;
    reg Ins_clock;
    reg RsHazardE;
    reg RtHazardE;
    reg RsHazardM;
    reg RtHazardM;
    reg Jump;
    reg Stall;
    integer f_addr;
    integer i;
    integer f_gr_addr;

    reg [1:0] JControlD;
    reg [1:0] JControlE;
    reg [1:0] JControlM;
    reg [1:0] JControlW;
    reg [31:0]JaddrD;
    reg [31:0]JaddrE;
    reg [31:0]JaddrM;
    reg [31:0]JaddrW;
    reg [31:0]JSrcE;
    reg [31:0]JSrcM;
    reg [31:0]JPCE;
    reg [31:0]JPCM;
    reg JRControl;
    reg JRControlE;
    reg JRControlM;


// 初始化
always @ (start)
begin

    MemtoRegD = 1'b0;
    MemtoRegE = 1'b0;
    MemtoRegM = 1'b0;
    MemtoRegW = 1'b0;
    RegWriteD = 1'b0;
    RegWriteE = 1'b0;
    RegWriteM = 1'b0;
    RegWriteW = 1'b0;
    RsHazardE = 1'b0;
    RtHazardE = 1'b0;
    RsHazardM = 1'b0;
    RtHazardM = 1'b0;
    Jump = 1'b0;
    RegDstD = 1'b0;
    RegDstE = 1'b0;
    ZeroE = 1'b0;
    ZeroM = 1'b0;
    BranchD = 1'b0;
    BranchE = 1'b0;
    BranchM = 1'b0;
    PCF = 32'b0;      //Program Counter
    PCPlus4F = 32'b0;
    PCPlus4D = 32'b0;
    PCPlus4E = 32'b0;
    PCbar = 32'b0;      //Program Counter ' 
    gr[0] = 32'b0;              //Zero应该最开始被赋0
    ALUOutE = 32'b0;
    ALUOutM = 32'b0;
    ALUOutW = 32'b0;
    ReadDataW = 32'b0;
    ResultW = 32'b0;
    WD3 = 32'b0;
    A1 = 5'b0;
    A2 = 5'b0;
    RD = 32'b0;
    RD1 = 32'b0;        
    RD2D = 32'b0;

    RD2E = 32'b0;
    SrcAE = 32'b0;
    SrcBE = 32'b0;
    PCBranchE = 32'b0;
    PCBranchM = 32'b0;
    SignImmD = 32'b0;
    SignImmE = 32'b0;
    ShiftImm = 32'b0;
    Mem_clock = 1'b0;
    Ins_clock = 1'b0;
    Stall = 1'b0;
    JSrcM = 32'b0;
    JRControl = 1'b0;
    JPCE = 32'b0;
    JPCM = 32'b0;
    JSrcE = 32'b0;
    JSrcM = 32'b0;
    JaddrD = 32'b0;
    JaddrE = 32'b0;
    JaddrM = 32'b0;
    JaddrW = 32'b0;
    JControlD = 2'b0;
    JControlE = 2'b0;
    JControlM = 2'b0;
    JControlW = 2'b0;
    JRControlE = 1'b0;
    JRControlM = 1'b0;


end

InstructionRAM a(.CLOCK(Ins_clock),.RESET(1'b0),.ENABLE(1'b1),.FETCH_ADDRESS(PCF));
MainMemory b(.CLOCK(Mem_clock), .RESET(1'b0),.ENABLE(1'b1),.FETCH_ADDRESS(ALUOutM),.EDIT_SERIAL({MemWriteM,ALUOutM>>2,WriteDataM}));

// Cycle 1, 获取InstructionRAM里的地址
always @ (posedge clock)
    begin
        if(((RsHazardE == 1'b1) || (RtHazardE == 1'b1)) && (Stall))
        begin
            Stall <= 0;
        end
        
        else
        begin
            PCF <= PCbar;
        end
        
    end

//当nonblocking statement被激活之后(即数据已被传输至PCF时，执行以下命令)
always @ (PCF)
    begin      
        Ins_clock = !Ins_clock;
        RD = a.RAM[PCF];
        PCPlus4F = PCF + 1;
    end

// Cycle 2，拉取register_file内的gr content并输出
always @ (posedge clock)
    begin
        case(PCSrcM)
        1'b1:
        begin
            InstrD <= 32'b0;
            PCPlus4D <= 32'b0;
        end
        
        default:
        begin
            if(((RsHazardE == 1'b1) || (RtHazardE == 1'b1)) && (Stall))
            begin
                Stall <= 0;
            end

            else
            begin
                InstrD <= RD;
                PCPlus4D <= PCPlus4F;
            end
        end

        endcase
    end

always @ (negedge clock)
    begin
        A1 = InstrD[25:21];
        A2 = InstrD[20:16];
        RD1 = gr[A1];
        RD2D = gr[A2];

        PCbar = PCSrcM ? PCBranchM : PCPlus4F;

        //读完之后，我们需要进行Forwarding操作
        if(RsHazardE == 1'b1 && (Stall != 1'b1))
        begin
            RD1 = ALUOutE;
        end

        else if(RsHazardM == 1'b1 && (Stall != 1'b1))
        begin
            RD1 = MemtoRegM ?  b.DATA : ALUOutM;
        end

        if(RtHazardE == 1'b1 && (Stall != 1'b1))
        begin
            RD2D = ALUOutE;
        end

        else if(RtHazardM == 1'b1 && (Stall != 1'b1))
        begin
            RD2D = MemtoRegM ?  b.DATA : ALUOutM;
        end
    end

//当nonblocking statement被激活之后(即数据已被传输至Instr或PCPlus4D时，执行以下命令)
always @ (InstrD , PCPlus4D , ResultW , WriteRegW)
    begin
        
        A3 = WriteRegW;
        WD3 = ResultW;
        shamtD = InstrD[10:6];
        SignImmD =  {{16{InstrD[15]}}, InstrD[15:0]};     //Sign-extended
        Op = InstrD[31:26];
        Funct = InstrD[5:0];
        JaddrD = {{6{1'b0}},InstrD[25:0]};
        JRControlE = 1'b0;
        MemWriteD = 1'b0;
        RegWriteD = 1'b0;

        case(RegWriteW)
            1'b1:
            begin
                gr[A3] = WD3;
            end
        endcase

        // Implementation of Control Unit.
        case(Op)
            //R-Type instructions
            6'b000000:
            begin
                JControlD = 2'b0; 
                case(Funct)
                    //add, Add(with overflow), regA for rs, regB for rt
                    6'b100000:
                    begin
                        ALUControlD = 5'b0;
                        ALUSrcD = 1'b0;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b1;
                        RegDstD = 1'b1;
                        BranchD = 1'b0;
                    end

                    //addu, Add unsigned (no overflow), regA for rs, regB for rt
                    6'b100001:
                    begin
                        ALUControlD = 5'b1;
                        ALUSrcD = 1'b0;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b1;
                        RegDstD = 1'b1;
                        BranchD = 1'b0;
                    end

                    //and, Bitwise and, regA for rs, regB for rt
                    6'b100100:
                    begin
                        ALUControlD = 5'b10;
                        ALUSrcD = 1'b0;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b1;
                        RegDstD = 1'b1;
                        BranchD = 1'b0;
                    end

                    //nor, Bitwise nor, regA for rs, regB for rt
                    6'b100111:
                    begin
                        ALUControlD = 5'b11;
                        ALUSrcD = 1'b0;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b1;
                        BranchD = 1'b0;
                        RegDstD = 1'b1;
                    end

                    //or, Bitwise or, regA for rs, regB for rt
                    6'b100101:
                    begin
                        ALUControlD = 5'b100;
                        ALUSrcD = 1'b0;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b1;
                        BranchD = 1'b0;
                        RegDstD = 1'b1;
                    end

                    //sll, Shift left logical, regA for rt, regB for sa !!!
                    6'b000000:
                    begin
                        ALUControlD = 5'b101;
                        ALUSrcD = 1'b0;
                        SaCtrlD = 1'b1;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b1;
                        BranchD = 1'b0;
                        RegDstD = 1'b1;
                    end

                    //sllv, shift left logical variable, regA for rt, regB for rs !!!
                    6'b000100:
                    begin
                        ALUControlD = 5'b110;
                        ALUSrcD = 1'b0;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b1;
                        BranchD = 1'b0;
                        RegDstD = 1'b1;
                    end

                    //slt, Set on less than (signed)
                    6'b101010:
                    begin
                        ALUControlD = 5'b111;
                        ALUSrcD = 1'b0;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b1;
                        BranchD = 1'b0;
                        RegDstD = 1'b1;
                    end


                    //sra, Shift right arithmetic
                    6'b000011:
                    begin
                        ALUControlD = 5'b1000;
                        ALUSrcD = 1'b0;
                        SaCtrlD = 1'b1;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b1;
                        BranchD = 1'b0;
                        RegDstD = 1'b1;
                    end

                    //srav, Shift right arithmetic variable
                    6'b000111:
                    begin
                        ALUControlD = 5'b1001;
                        ALUSrcD = 1'b0;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b1;
                        BranchD = 1'b0;
                        RegDstD = 1'b1;
                    end

                    //srl, Shift right logical
                    6'b000010:
                    begin
                        ALUControlD = 5'b1010;
                        ALUSrcD = 1'b0;
                        SaCtrlD = 1'b1;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b1;
                        BranchD = 1'b0;
                        RegDstD = 1'b1;
                    end

                    //srlv, Shift right logical variable
                    6'b000110:
                    begin
                        ALUControlD = 5'b1011;
                        ALUSrcD = 1'b0;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b1;
                        BranchD = 1'b0;
                        RegDstD = 1'b1;
                    end

                    //sub, Sub(with overflow), regA for rs, regB for rt
                    6'b100010:
                    begin
                        ALUControlD = 5'b1100;
                        ALUSrcD = 1'b0;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b1;
                        BranchD = 1'b0;
                        RegDstD = 1'b1;
                    end

                    //subu, Sub unsigned (no overflow), regA for rs, regB for rt
                    6'b100011:
                    begin
                        ALUControlD = 5'b1101;
                        ALUSrcD = 1'b0;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b1;
                        BranchD = 1'b0;
                        RegDstD = 1'b1;
                    end

                    //xor, Bitwise exclusive or, regA for rs, regB for rt
                    6'b100110:
                    begin
                        ALUControlD = 5'b1110;
                        ALUSrcD = 1'b0;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b1;
                        BranchD = 1'b0;
                        RegDstD = 1'b1;
                    end

                    //jr, Jump register
                    6'b001000:
                    begin
                        ALUControlD = 5'b01111;
                        ALUSrcD = 1'b0;
                        MemWriteD = 1'b0;
                        MemtoRegD = 1'b0;
                        RegWriteD = 1'b0;
                        BranchD = 1'b0;
                        JControlD = 2'b01; 
                    end
                endcase
            end


            //I-type instructions

            //addi, add immediate with overflow
            6'b001000:
            begin
                JControlD = 2'b0; 
                ALUControlD = 5'b0;
                ALUSrcD = 1'b1;
                SaCtrlD = 1'b0;
                MemWriteD = 1'b0;
                MemtoRegD = 1'b0;
                RegWriteD = 1'b1;
                RegDstD = 1'b0;
                BranchD = 1'b0;
            end

            //addiu, Add immediate unsigned (no overflow), regA for rs, regB for rt
            6'b001001:
            begin
                JControlD = 2'b0; 
                ALUControlD = 5'b1;
                ALUSrcD = 1'b1;
                SaCtrlD = 1'b0;
                MemWriteD = 1'b0;
                MemtoRegD = 1'b0;
                RegWriteD = 1'b1;
                RegDstD = 1'b0;
                BranchD = 1'b0;
            end

            //andi, Bitwise immediate and, regA for rs, regB for rt
            6'b001100:
            begin
                JControlD = 2'b0; 
                ALUControlD = 5'b10;
                ALUSrcD = 1'b1;
                SaCtrlD = 1'b0;
                MemWriteD = 1'b0;
                MemtoRegD = 1'b0;
                RegWriteD = 1'b1;
                RegDstD = 1'b0;
                BranchD = 1'b0;
                SignImmD = {{16{1'b0}}, SignImmD[15:0]};
            end

            //beq, Branch on equal, equivalent to sub
            6'b000100:
            begin
                JControlD = 2'b0; 
                ALUControlD = 5'b10000;
                ALUSrcD = 1'b0;
                SaCtrlD = 1'b0;
                MemWriteD = 1'b0;
                MemtoRegD = 1'b0;
                RegWriteD = 1'b0;
                BranchD = 1'b1;
            end

            //bne, Branch on not equal, equivalent to sub
            6'b000101:
            begin
                JControlD = 2'b0; 
                ALUControlD = 5'b10001;
                ALUSrcD = 1'b0;
                SaCtrlD = 1'b0;
                MemWriteD = 1'b0;
                MemtoRegD = 1'b0;
                RegWriteD = 1'b0;  //因为不用RegWrite，所以自然没有declare RegDsc的必要
                BranchD = 1'b1;
            end

            //lw, load word, equivalent to add immediate with overflow
            6'b100011:
            begin
                JControlD = 2'b0; 
                ALUControlD = 5'b10010;
                ALUSrcD = 1'b1;
                SaCtrlD = 1'b0;
                MemWriteD = 1'b0;
                MemtoRegD = 1'b1;
                RegWriteD = 1'b1;
                RegDstD = 1'b0;
                BranchD = 1'b0;
            end

            //sw, save word, equivalent to add immediate with overflow
            6'b101011:
            begin
                JControlD = 2'b0; 
                ALUControlD = 5'b10011;
                ALUSrcD = 1'b1;
                SaCtrlD = 1'b0;
                MemWriteD = 1'b1;
                MemtoRegD = 1'b0;
                RegWriteD = 1'b0;
                BranchD = 1'b0;
            end

            //ori, Bitwise or immediate, regA for rs, regB for rt
            6'b001101:
            begin
                JControlD = 2'b0; 
                ALUControlD = 5'b100;
                ALUSrcD = 1'b1;
                SaCtrlD = 1'b0;
                MemWriteD = 1'b0;
                MemtoRegD = 1'b0;
                RegWriteD = 1'b1;
                RegDstD = 1'b0;
                BranchD = 1'b0;
                SignImmD = {{16{1'b0}}, SignImmD[15:0]};
            end

            //xori, Bitwise exclusive or immediate, regA for rs, regB for rt
            6'b001110:
            begin
                JControlD = 2'b0; 
                ALUControlD = 5'b1110;
                ALUSrcD = 1'b1;
                SaCtrlD = 1'b0;
                MemWriteD = 1'b0;
                MemtoRegD = 1'b0;
                RegWriteD = 1'b1;
                RegDstD = 1'b0;
                BranchD = 1'b0;
                SignImmD = {{16{1'b0}}, SignImmD[15:0]};
            end

            //j, Jump 
            6'b000010:
            begin
                JControlD = 2'b11; 
                ALUControlD = 5'b11110;
                ALUSrcD = 1'b1;
                SaCtrlD = 1'b0;
                MemWriteD = 1'b0;
                MemtoRegD = 1'b0;
                RegWriteD = 1'b0;
                BranchD = 1'b0;
            end

            //jal, Jump and link   
            6'b000011:
            begin
                JControlD = 2'b10; 
                ALUControlD = 5'b11111;
                ALUSrcD = 1'b1;
                SaCtrlD = 1'b0;
                MemWriteD = 1'b0;
                MemtoRegD = 1'b0;
                RegWriteD = 1'b1;
                BranchD = 1'b0;
            end
            
        endcase
    end

// Clock 3, 进行ALU运算
always @ (posedge clock)
    begin
        case(PCSrcM)
        1'b1:
        begin
            JControlE <= 2'b0;
            JaddrE <= 32'b0;
            ALUControlE <= 5'b0;
            ALUSrcE <= 1'b0;
            SignImmE <= 32'b0;
            SrcAE <= 32'b0;
            RD2E <= 32'b0;
            shamtE <= 5'b0;
            MemWriteE <= 1'b0;
            MemtoRegE <= 1'b0;
            RegWriteE <= 1'b0;
            RtE <= 5'b0;
            RdE <= 5'b0;
            RegDstE <= 1'b0;
            BranchE <= 1'b0;
            PCPlus4E <= 32'b0;
            
        end

        default:
        begin
            if(((RsHazardE == 1'b1) || (RtHazardE == 1'b1)) && (Stall))
            begin
                Stall <= 0;
                JControlE <= 2'b0;
            end
            else
            begin
                JControlE <= JControlD;
            end

            JaddrE <= JaddrD;
            ALUControlE <= ALUControlD;
            ALUSrcE <= ALUSrcD;
            SignImmE <= SignImmD;
            SrcAE <= RD1;
            RD2E <= RD2D;
            shamtE <= shamtD;
            MemWriteE <= MemWriteD;
            MemtoRegE <= MemtoRegD;
            RegWriteE <= RegWriteD;
            RtE <= InstrD[20:16];
            RdE <= InstrD[15:11];
            RegDstE <= RegDstD;
            BranchE <= BranchD;
            PCPlus4E <= PCPlus4D;
        end

        endcase
    end

always @ (ALUControlE , ALUSrcE , SignImmE 
        , SrcAE , shamtE , WriteDataE, RD2E,
        RtE , RdE , RegDstE , PCPlus4E)
    
    begin
        WriteDataE = RD2E;
        SrcBE = ALUSrcE ? SignImmE : RD2E;
        ShiftImm = SignImmE - 32'b00000000000000000000000000000001;
        PCBranchE = ShiftImm + PCPlus4E;
        WriteRegE = RegDstE ? RdE : RtE;
        JSrcE = SrcAE;
        JPCE = PCPlus4E;
        
        //以下为ALU的具体计算过程，结果待检测
        case(ALUControlE)
            // Add
            5'b00000:
            begin
                {extension, ALUOutE} = {SrcAE[31],SrcAE} + {SrcBE[31],SrcBE};
                overflow = extension ^ ALUOutE[31];    //Configure overflow
            end

            //addu, Add unsigned (no overflow), regA for rs, regB for rt
            5'b00001:
            begin
                ALUOutE = SrcAE + SrcBE;
            end

            //and, Bitwise and, regA for rs, regB for rt
            5'b00010:
            begin
                ALUOutE = SrcAE & SrcBE;
            end

            //nor, Bitwise nor, regA for rs, regB for rt
            5'b11:
            begin
                ALUOutE = SrcAE ~| SrcBE;
            end

            //or, Bitwise or, regA for rs, regB for rt
            5'b100:
            begin
                ALUOutE = SrcAE | SrcBE;
            end

            //sll, Shift left logical, regA for rt, regB for sa !!!
            5'b101:
            begin
                SrcAE = {{27{1'b0}}, shamtE};
                ALUOutE = SrcBE << SrcAE;
            end

            //sllv, shift left logical variable, regA for rt, regB for rs !!!
            5'b110:
            begin
                ALUOutE = SrcBE << SrcAE;
            end

            //slt, Set on less than (signed)
            5'b111:
            begin
                ALUOutE = (SrcAE < SrcBE) ? 1'b1:1'b0;
            end

            //sra, Shift right arithmetic
            5'b1000:
            begin
                SrcAE = {{27{1'b0}}, shamtE};
                sign_SrcAE = SrcAE;
                sign_SrcBE = SrcBE;
                ALUOutE = sign_SrcBE >>> sign_SrcAE;
            end

            //srav, Shift right arithmetic variable
            5'b1001:
            begin
                sign_SrcAE = SrcAE;
                sign_SrcBE = SrcBE;
                ALUOutE = sign_SrcBE >>> sign_SrcAE;
            end

            //srl, Shift right logical
            5'b1010:
            begin
                SrcAE = {{27{1'b0}}, shamtE};
                ALUOutE = SrcBE >> SrcAE;
            end

            //srlv, Shift right logical variable
            5'b1011:
            begin
                ALUOutE = SrcBE >> SrcAE;
            end

            //sub, Sub(with overflow), regA for rs, regB for rt
            5'b1100:
            begin
                {extension, ALUOutE} = {SrcAE[31],SrcAE} - {SrcBE[31],SrcBE};
                overflow = extension ^ ALUOutE[31];    //Configure overflow
            end

            //subu, Sub unsigned (no overflow), regA for rs, regB for rt
            5'b1101:
            begin
                ALUOutE = SrcAE - SrcBE;
            end

            //xor, Bitwise exclusive or, regA for rs, regB for rt
            5'b1110:
            begin
                ALUOutE = SrcAE ^ SrcBE;
            end

            //jr, Jump register
            5'b01111:
            begin
                {extension, ALUOutE} = {SrcAE[31],SrcAE} - {SrcBE[31],SrcBE};
                overflow = extension ^ ALUOutE[31];    //Configure overflow
                JRControlE <= 1'b1;
            end

            //beq, Branch on equal, equivalent to sub
            5'b10000:
            begin
                {extension, ALUOutE} = {SrcAE[31],SrcAE} - {SrcBE[31],SrcBE};
                overflow = extension ^ ALUOutE[31];    //Configure overflow
                case(ALUOutE)
                    32'b0:
                    begin
                        ZeroE = 1'b1;
                    end
                    default:
                    begin
                        ZeroE = 1'b0;
                    end
                endcase
            end

            //bne, Branch on not equal, equivalent to sub
            5'b10001:
            begin
                {extension, ALUOutE} = {SrcAE[31],SrcAE} - {SrcBE[31],SrcBE};
                overflow = extension ^ ALUOutE[31];    //Configure overflow
                case(ALUOutE)
                    32'b0:
                    begin
                        ZeroE = 1'b0;
                    end
                    default:
                    begin
                        ZeroE = 1'b1;
                    end
                endcase
            end

            //lw, load word, equivalent to add immediate with overflow
            5'b10010:
            begin
                {extension, ALUOutE} = {SrcAE[31],SrcAE} + {SrcBE[31],SrcBE};
                overflow = extension ^ ALUOutE[31];    //Configure overflow
                Stall = 1'b1;
            end

            //sw, save word, equivalent to add immediate with overflow
            5'b10011:
            begin
                {extension, ALUOutE} = {SrcAE[31],SrcAE} + {SrcBE[31],SrcBE};
                overflow = extension ^ ALUOutE[31];    //Configure overflow
            end
        endcase

        //判断是否出现Hazard
        if((RegWriteE == 1'b1) && (InstrD[25:21] != 5'b0) && (InstrD[25:21] == WriteRegE))
        begin
            //$display("Yes");
            RsHazardE = 1'b1;
        end

        else
        begin
            RsHazardE = 1'b0;
        end

        if((RegWriteE == 1'b1) && (InstrD[20:16] != 5'b0) && (InstrD[20:16] == WriteRegE))
        begin
            //$display("Yes");
            RtHazardE = 1'b1;
        end

        else
        begin
            RtHazardE = 1'b0;
        end

    end

// Cycle 4, Read / Write data into data memory.
always @ (posedge clock)
    begin
        case(PCSrcM)
        1'b1:
        begin
            MemWriteM <= 1'b0;
            MemtoRegM <= 1'b0;
            RegWriteM <= 1'b0;
            ALUOutM <= 32'b0;
            WriteDataM <= 1'b0;
            BranchM <= 1'b0;
            ZeroM <= 1'b0;
            WriteRegM <= 1'b0;
            PCBranchM <= 32'b0;
            JControlM <= 2'b0;
            JaddrM <= 32'b0;
            JSrcM <= 32'b0;
            JPCM <= 32'b0;
            JRControlM <= 1'b0;
        end

        default:
        begin
            MemWriteM <= MemWriteE;
            MemtoRegM <= MemtoRegE;
            RegWriteM <= RegWriteE;
            ALUOutM <= ALUOutE;
            WriteDataM <= WriteDataE;
            BranchM <= BranchE;
            ZeroM <= ZeroE;
            WriteRegM <= WriteRegE;
            PCBranchM <= PCBranchE;
            JControlM <= JControlE;
            JaddrM = JaddrE;
            JSrcM <= JSrcE;
            JPCM <= JPCE;
            JRControlM <= JRControlE;
        end

        endcase
    end

//括号中为实际业务中落地的部分，即always中将被使用的变量或在Data memory中被调用的变量。
always @ (PCPlus4F, ALUOutM)
    begin
        PCSrcM = BranchM & ZeroM;

        //jal和j的具体设置
        if((JControlM == 2'b10) || (JControlM == 2'b11))
        begin

            PCSrcM = 1'b1;
            PCBranchM = JaddrM;
            if(JControlM == 2'b10)
            begin
                ALUOutM = JPCM << 2;
                WriteRegM = 5'b11111;
            end
        end

        //jr的具体设置
        else if(JControlM == 2'b01)
        begin
            PCSrcM = 1'b1;
            PCBranchM = (JSrcM >> 2);
        end

        Ins_clock = !Ins_clock;
        PCbar = PCSrcM ? PCBranchM : PCPlus4F;
        Mem_clock = !Mem_clock;

        //判断是否出现Hazard
        if((RegWriteM == 1'b1) && (InstrD[25:21] != 5'b0) && (InstrD[25:21] == WriteRegM))
        begin
            RsHazardM = 1'b1;
        end

        else
        begin
            RsHazardM = 1'b0;
        end

        if((RegWriteM == 1'b1) && (InstrD[20:16] != 5'b0) && (InstrD[20:16] == WriteRegM))
        begin
            RtHazardM = 1'b1;
        end

        else
        begin
            RtHazardM = 1'b0;
        end
    end

// Cycle 5, Make decisions
always @ (posedge clock)
    begin
        WriteRegW <= WriteRegM;
        ReadDataW <= b.DATA;
        ALUOutW <= ALUOutM;
        MemtoRegW <= MemtoRegM;
        RegWriteW <= RegWriteM;
    end

always @ (ALUOutW , ReadDataW)
    begin
        ResultW = MemtoRegW ? ReadDataW : ALUOutW;
    end

endmodule