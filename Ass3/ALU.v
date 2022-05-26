module alu(instruction,regA, regB, result, flags);

//Input & output declaration 
input signed [31:0] instruction,regA, regB;
output signed [31:0] result;
output [2:0] flags;

//Localization Registers
reg[5:0] opcode, func;
reg[4:0] rs, rt, rd;
reg signed[31:0] reg_A, reg_B, reg_C, rs_reg, rt_reg;
reg[31:0] unsigned_reg_A, unsigned_reg_B;   //Pointer casting?
reg extension;
reg zero;
reg negative;
reg overflow;

//reg_C stands for temporary storage reg for result.

always @(instruction,regA, regB)
begin

    opcode = instruction[31:26];
    func = instruction[5:0];
    rs = instruction[25:21];
    rt = instruction[20:16];
    rd = instruction[15:11];

    //Register fetch
    case(rs)
        5'b00000:
        begin
            rs_reg = regA;
        end
        5'b00001:
        begin
            rs_reg = regB;
        end
    endcase

    case(rt)
        5'b00000:
        begin
            rt_reg = regA;
        end
        5'b00001:
        begin
            rt_reg = regB;
        end
    endcase

    case(opcode)

        //R-Type instructions
        6'b000000:
        begin

            case(func)

                //add, Add(with overflow), regA for rs, regB for rt
                6'b100000:
                begin
                    {extension, reg_C} = {rs_reg[31],rs_reg} + {rt_reg[31],rt_reg};
                    overflow = extension ^ reg_C[31];    //Configure overflow
                end

                //addu, Add unsigned (no overflow), regA for rs, regB for rt
                6'b100001:
                begin
                    unsigned_reg_A = rs_reg;
                    unsigned_reg_B = rt_reg;
                    reg_C = unsigned_reg_A + unsigned_reg_B;
                end

                //and, Bitwise and, regA for rs, regB for rt
                6'b100100:
                begin
                    reg_A = rs_reg;
                    reg_B = rt_reg;
                    reg_C = reg_A & reg_B;
                end

                //nor, Bitwise nor, regA for rs, regB for rt
                6'b100111:
                begin
                    reg_A = rs_reg;
                    reg_B = rt_reg;
                    reg_C = reg_A ~| reg_B;
                end

                //or, Bitwise or, regA for rs, regB for rt
                6'b100101:
                begin
                    reg_A = rs_reg;
                    reg_B = rt_reg;
                    reg_C = reg_A | reg_B;
                end

                //sll, Shift left logical, regA for rt, regB for sa !!!
                6'b000000:
                begin
                    reg_A = rt_reg;
                    reg_B = {{27{1'b0}}, instruction[10:6]};
                    reg_C = reg_A << reg_B;
                end

                //sllv, shift left logical variable, regA for rt, regB for rs !!!
                6'b000100:
                begin
                    reg_A = rt_reg;
                    reg_B = rs_reg;
                    reg_C = reg_A << reg_B;
                end

                //slt, Set on less than (signed)
                6'b101010:
                begin
                    {extension, reg_C} = {rs_reg[31],rs_reg} - {rt_reg[31],rt_reg};
                    overflow = extension ^ reg_C[31];    //Configure overflow
                end

                //sltu, Set on less than unsigned
                6'b101011:
                begin
                    unsigned_reg_A = rs_reg;
                    unsigned_reg_B = rt_reg;
                    reg_C = unsigned_reg_A - unsigned_reg_B;
                end

                //sra, Shift right arithmetic
                6'b000011:
                begin
                    reg_A = rt_reg;
                    reg_B = {{27{1'b0}}, instruction[10:6]};
                    reg_C = reg_A >>> reg_B;
                end

                //srav, Shift right arithmetic variable
                6'b000111:
                begin
                    reg_A = rt_reg;
                    reg_B = rs_reg;
                    reg_C = reg_A >>> reg_B;
                end

                //srl, Shift right logical
                6'b000010:
                begin
                    reg_A = rt_reg;
                    reg_B = {{27{1'b0}}, instruction[10:6]};
                    reg_C = reg_A >> reg_B;
                end

                //srlv, Shift right logical variable
                6'b000110:
                begin
                    reg_A = rt_reg;
                    reg_B = rs_reg;
                    reg_C = reg_A >> reg_B;
                end

                //sub, Sub(with overflow), regA for rs, regB for rt
                6'b100010:
                begin
                    reg_A = rs_reg;
                    reg_B = rt_reg;
                    {extension, reg_C} = {rs_reg[31],rs_reg} - {rt_reg[31],rt_reg};
                    overflow = extension ^ reg_C[31];    //Configure overflow
                end

                //subu, Sub unsigned (no overflow), regA for rs, regB for rt
                6'b100011:
                begin
                    unsigned_reg_A = rs_reg;
                    unsigned_reg_B = rt_reg;
                    reg_C = unsigned_reg_A - unsigned_reg_B;
                end

                //xor, Bitwise exclusive or, regA for rs, regB for rt
                6'b100110:
                begin
                    reg_A = rs_reg;
                    reg_B = rt_reg;
                    reg_C = reg_A ^ reg_B;
                end
                

            endcase
        end

        //I-type instructions

        //addi, add immediate with overflow
        6'b001000:
        begin
            reg_A = rs_reg;
            reg_B = {{16{instruction[15]}}, instruction[15:0]};
            {extension, reg_C} = {reg_A[31],reg_A} + {reg_B[31],reg_B};
            overflow = extension ^ reg_C[31];    //Configure overflow
        end

        //addiu, Add immediate unsigned (no overflow), regA for rs, regB for rt
        6'b001001:
        begin
            unsigned_reg_A = rs_reg;
            unsigned_reg_B = {{16{instruction[15]}}, instruction[15:0]};
            reg_C = unsigned_reg_A + unsigned_reg_B;
        end

        //andi, Bitwise immediate and, regA for rs, regB for rt
        6'b001100:
        begin
            reg_A = rs_reg;
            reg_B = {{16{1'b0}}, instruction[15:0]};
            reg_C = reg_A & reg_B;
        end

        //beq, Branch on equal, equivalent to sub
        6'b000100:
        begin
            reg_A = rs_reg;
            reg_B = rt_reg;
            {extension, reg_C} = {reg_A[31],reg_A} - {reg_B[31],reg_B};
            overflow = extension ^ reg_C[31];    //Configure overflow
        end

        //bne, Branch on not equal, equivalent to sub
        6'b000101:
        begin
            reg_A = rs_reg;
            reg_B = rt_reg;
            {extension, reg_C} = {reg_A[31],reg_A} - {reg_B[31],reg_B};
            overflow = extension ^ reg_C[31];    //Configure overflow
        end

        //lw, load word, equivalent to add immediate with overflow
        6'b100011:
        begin
            reg_A = rs_reg;
            reg_B = {{16{instruction[15]}}, instruction[15:0]};
            {extension, reg_C} = {reg_A[31],reg_A} + {reg_B[31],reg_B};
            overflow = extension ^ reg_C[31];    //Configure overflow
        end

        //sw, save word, equivalent to add immediate with overflow
        6'b101011:
        begin
            reg_A = rs_reg;
            reg_B = {{16{instruction[15]}}, instruction[15:0]};
            {extension, reg_C} = {reg_A[31],reg_A} + {reg_B[31],reg_B};
            overflow = extension ^ reg_C[31];    //Configure overflow
        end

        //ori, Bitwise or immediate, regA for rs, regB for rt
        6'b001101:
        begin
            reg_A = rs_reg;
            reg_B = {{16{1'b0}}, instruction[15:0]};
            reg_C = reg_A | reg_B;
        end

        //slti, Set on less than immediate (signed)
        6'b001010:
        begin
            reg_A = rs_reg;
            reg_B = {{16{instruction[15]}}, instruction[15:0]};
            {extension, reg_C} = {reg_A[31],reg_A} - {reg_B[31],reg_B};
            overflow = extension ^ reg_C[31];    //Configure overflow
        end

        //sltiu, Set on less than immediate unsigned
        6'b001011:
        begin
            unsigned_reg_A = rs_reg;
            unsigned_reg_B = {{16{instruction[15]}}, instruction[15:0]};
            reg_C = unsigned_reg_A - unsigned_reg_B;
        end

        //xori, Bitwise exclusive or immediate, regA for rs, regB for rt
        6'b001110:
        begin
            reg_A = rs_reg;
            reg_B = {{16{1'b0}}, instruction[15:0]};
            reg_C = reg_A ^ reg_B;
        end

    endcase

    //Configuration of zero
    case (reg_C)
        32'b0: 
        begin
            zero = 1'b1;
        end
        default: 
        begin
            zero = 1'b0;
        end
    endcase

    //Configuration of negative
    case (reg_C[31])
        1'b1: 
        begin
            negative = 1'b1;
        end
        default: 
        begin
            negative = 1'b0;
        end
    endcase

end

assign result = reg_C[31:0];
assign flags[2] = zero;     //Configuration of zero
assign flags[1] = negative;     //Configuration of negative
assign flags[0] = overflow;     //Configuration of overflow


endmodule