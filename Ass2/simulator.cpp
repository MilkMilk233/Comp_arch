#include <iostream>
#include <set>
#include <cstdio>
#include <malloc.h>
#include <math.h>
#include <fstream>
#include <sstream>
#include <map>

//File I/O related APIs
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

ifstream txt_file;
ifstream asm_file;
ifstream checkpoint;
ifstream read_in;
ofstream write_out;

int bin_to_dec(string binary_num);

//Memory Simulation part, pointers belows pointing to virtual memory address.

int * memory_pointer;   //Which records the location of main memory.
int * register_baseline;    //Which records the location of registers.
char * dynamic_data;
int * $zero;
int * $at;
int * $v0;
int * $v1;
int * $a0;
int * $a1;
int * $a2;
int * $a3;
int * $t0;
int * $t1;
int * $t2;
int * $t3;
int * $t4;
int * $t5;
int * $t6;
int * $t7;
int * $s0;
int * $s1;
int * $s2;
int * $s3;
int * $s4;
int * $s5;
int * $s6;
int * $s7;
int * $t8;
int * $t9;
int * $k0;
int * $k1;
int * $gp;
int * $sp;
int * $fp;
int * $ra;
int * $pc;
int * $lo;
int * $hi;

int * text_init_pt;
char * data_init_pt;
int * data_assist_pt;

int ins_count = 0;
int ascii_count;

static int flag = 0;    //0 Stands for continue, 1 stands for exit.

// checkpoint
std::set<int> checkpoints;
string checkpoint_string;
void init_checkpoints(){
    while(getline(checkpoint,checkpoint_string)){
        checkpoints.insert(atoi(checkpoint_string.c_str()));
    }
    checkpoint.close();
}

void checkpoint_memory(int ins_count){
    if (!checkpoints.count(ins_count)) return;
    std::string name = "memory_test_" + std::to_string(ins_count) + ".bin";
    FILE * fp = fopen(name.c_str(), "wb");
    fwrite(memory_pointer, 1, 0x600000, fp);
    fclose(fp);
}

void checkpoint_register(int ins_count) {
    if (!checkpoints.count(ins_count)) return;
    std::string name = "register_test_" + std::to_string(ins_count) + ".bin";
    FILE * fp = fopen(name.c_str(), "wb");
    fwrite(register_baseline, 1, 140, fp);
    fclose(fp);
}

void text_segment_init(){
    //FILE *fp = fopen(binary_code.c_str(), "r");
    string bin_string;
    int dec_output;
    text_init_pt = &*memory_pointer;
    while(getline(txt_file,bin_string)){
        dec_output = bin_to_dec(bin_string);
        *text_init_pt = dec_output;
        text_init_pt = &*text_init_pt+1;
    }
}

void static_data_init(){
    string data_str;
    string::size_type dataPos;
    string actual_str;
    string::size_type end;
    bool isData = false;
    data_init_pt = (char*) &*memory_pointer+0x100000;

    while(getline(asm_file,data_str)){
        //进入循环条件
        if(!isData && (dataPos = data_str.find(".data")) != data_str.npos){
            isData = true;
            continue;
        }
        //退出循环条件
        else if(isData && (dataPos = data_str.find(".text")) != data_str.npos){
            break;
        }   
        //处理每一行，将其写入虚拟内存         
        else{
            //选择最靠前的一个标签，确认本行类型
            
            string::size_type asciiz = data_str.find(".asciiz ");
            string::size_type ascii = data_str.find(".ascii ");
            
            
            string::size_type byte = data_str.find(".byte");
            string::size_type word = data_str.find(".word");
            string::size_type half = data_str.find(".half");
            int minimun_num = 10000;
            int minimun_index = 0;
            int starting_index;
            int ending_index;
            if(ascii < minimun_num){
                minimun_num = ascii;
            }
            if(asciiz < minimun_num){
                minimun_num = asciiz;
                minimun_index = 1;
            }
            if(byte < minimun_num){
                minimun_num = byte;
                minimun_index = 2;
            }
            if(word < minimun_num){
                minimun_num = word;
                minimun_index = 3;
            }
            if(half < minimun_num){
                minimun_num = half;
                minimun_index = 4;
            }

            if(minimun_num == 10000) continue;  //i.e. this is a comment or empty line.
        
            //Discard all the comments.
            if((dataPos = data_str.find("#")) != data_str.npos){
                data_str = data_str.substr(0,dataPos);
            }
            //ascii 和 asciiz 的区别：后者会在字符串后自动添加结束符\0。
            switch(minimun_index){
                case 0:
                    //For case ascii
                    dataPos = data_str.find("\"");
                    end = data_str.substr(dataPos+1, data_str.size()-dataPos-1).find("\"");
                    actual_str = data_str.substr(dataPos+1,end);
                    ascii_count = 0;
                    for(int i = 0; i<actual_str.size(); i++){
                        if(actual_str[i] == '\\'){
                            i++;
                            *data_init_pt = '\n';
                            data_init_pt = &*data_init_pt+1;
                            ascii_count++;
                        }
                        else{
                            *data_init_pt = actual_str[i];
                            data_init_pt = &*data_init_pt+1;
                            ascii_count++;
                        }
                            
                    }
                    for(int i = ascii_count; i%4 != 0; i++){
                        *data_init_pt = '\0';
                        data_init_pt = &*data_init_pt+1;
                        ascii_count++;
                    }
                    break;
                case 1:
                    //For case asciiz
                    dataPos = data_str.find("\"");
                    end = data_str.substr(dataPos+1, data_str.size()-dataPos-1).find("\"");
                    actual_str = data_str.substr(dataPos+1,end);
                    ascii_count = 0;
                    for(int i = 0; i<actual_str.size(); i++){
                        if(actual_str[i] == '\\'){
                            i++;
                            *data_init_pt = '\n';
                            data_init_pt = &*data_init_pt+1;
                            ascii_count++;
                        }
                        else {
                            *data_init_pt = actual_str[i];
                            data_init_pt = &*data_init_pt+1;
                            ascii_count++;
                        }
                    }
                    //在字符串后自动添加结束符\0。
                    *data_init_pt = '\0';
                    data_init_pt = &*data_init_pt+1;
                    ascii_count++;

                    for(int i = ascii_count; i%4 != 0; i++){
                        *data_init_pt = '\0';
                        data_init_pt = &*data_init_pt+1;
                        ascii_count++;
                    }
                    break;
                case 2:
                    //For the .byte case.
                    dataPos = data_str.find(".");
                    data_str = data_str.substr(dataPos+5,data_str.size()-dataPos-5);
                    starting_index = 0;
                    ascii_count = 0;
                    for(int i = 0; i<data_str.size(); i++){
                        if(data_str[i] == ','){
                            ending_index = i;
                            *data_init_pt = (char) atoi(data_str.substr(starting_index,ending_index-starting_index).c_str());
                            data_init_pt = &*data_init_pt+1;
                            starting_index = i+1;
                            ascii_count++;
                        }
                    }
                    ending_index = data_str.size();
                    *data_init_pt = (char) atoi(data_str.substr(starting_index,ending_index-starting_index).c_str());
                    data_init_pt = &*data_init_pt+1;
                    ascii_count++;
                    //自动补位,防止崩溃
                    for(int i = ascii_count; i%4 != 0; i++){
                        *data_init_pt = '\0';
                        data_init_pt = &*data_init_pt+1;
                        ascii_count++;
                    }
                    break;
                case 3:
                    //For .word case
                    dataPos = data_str.find(".");
                    data_str = data_str.substr(dataPos+5,data_str.size()-dataPos-5);
                    starting_index = 0;
                    for(int i = 0; i<data_str.size(); i++){
                        if(data_str[i] == ','){
                            ending_index = i;
                            data_assist_pt = (int*) &*data_init_pt;
                            *data_assist_pt = atoi(data_str.substr(starting_index,ending_index-starting_index).c_str());
                            data_init_pt = &*data_init_pt+4;
                            starting_index = i+1;
                        }
                    }
                    ending_index = data_str.size();
                    data_assist_pt = (int*) &*data_init_pt;
                    *data_assist_pt = atoi(data_str.substr(starting_index,ending_index-starting_index).c_str());
                    data_init_pt = &*data_init_pt+4;
                    break;
                case 4:
                    //For the .half case
                    dataPos = data_str.find(".");
                    data_str = data_str.substr(dataPos+5,data_str.size()-dataPos-5);
                    starting_index = 0;
                    ascii_count = 0;
                    for(int i = 0; i<data_str.size(); i++){
                        if(data_str[i] == ','){
                            ending_index = i;
                            data_assist_pt = (int*) &*data_init_pt;
                            *data_assist_pt = atoi(data_str.substr(starting_index,ending_index-starting_index).c_str());
                            data_init_pt = &*data_init_pt+2;
                            starting_index = i+1;
                            ascii_count += 2;
                        }
                    }
                    ending_index = data_str.size();
                    data_assist_pt = (int*) &*data_init_pt;
                    *data_assist_pt = atoi(data_str.substr(starting_index,ending_index-starting_index).c_str());
                    data_init_pt = &*data_init_pt+2;
                    ascii_count += 2;
                    for(int i = ascii_count; i%4 != 0; i++){
                        *data_init_pt = '\0';
                        data_init_pt = &*data_init_pt+1;
                        ascii_count += 2;
                    }

                    break;
            }

        }
    }

    dynamic_data = data_init_pt;
    
}

//Unsigned
int bin_to_dec(string binary_num){
    int count = 0;
    int result = 0;
    for(int i = binary_num.size()-1; i>=0; i--){
        if(binary_num[i] == '1'){
            result += (int) pow(2,count);
        }
        count++;
    }
    return result;
}

//Signed
int bin_to_dec_signed(string binary_num){
    int count = 0;
    int result = 0;
    bool is_negative = false;
    if(binary_num[0] == '1') is_negative = true;

    //Perform 2's complement.
    bool invert = false;
    if(is_negative){
        for(int i = binary_num.size()-1; i>=0; i--){
            if(binary_num[i] == '1'){
                if(invert == false) invert = true;
                else binary_num[i] = '0';
            }
            else{
                if(invert) binary_num[i] = '1';
            }
        }
    }

    for(int i = binary_num.size()-1; i>=0; i--){
        if(binary_num[i] == '1'){
            result += (int) pow(2,count);
        }
        count++;
    }
    
    if(is_negative) result = -result;

    return result;
}

string sign_extent(string source, int target_bit, char fulfill_with){
    stringstream model;
    for(int i = 0; i < target_bit - source.size(); i++){
        model << fulfill_with;
    }
    return model.str() + source;
}

string dec_to_bin(int dec_num){
    string binary;
    bool is_negative = false;
    char sign_extent_char = '0';
    if(dec_num == 0){
        binary = "0";
    }
    else{
        if(dec_num < 0){
            is_negative = true;
            dec_num = -dec_num;
            sign_extent_char = '1';
        }
        int n = dec_num;
        stringstream sss;
        int remainder = 1;
        //Decimal to binary.
        while(true){
            remainder = (int) n%2;
            sss<<remainder;
            n = (int) n/2;
            if(remainder == 1 && n == 0) break;
        }
        binary = sss.str();
        sss.str("");
        for(int k = binary.size()-1; k>=0; k--){
            sss<<binary[k];
        }
        binary = sss.str();
        //Perform 2's complement.
        bool invert = false;
        if(is_negative){
            for(int i = binary.size()-1; i>=0; i--){
                if(binary[i] == '1'){
                    if(invert == false) invert = true;
                    else binary[i] = '0';
                }
                else{
                    if(invert) binary[i] = '1';
                }
            }
        }
    }

    //Sign-Extent
    return sign_extent(binary,32,sign_extent_char);
}

//Execute current line pc pointing to, jump pc. Return 1 whenever exit, otherwise return 0;
void execute(){
    
    int * pc_real_addr();
    int * str_to_reg(string bin_codes);
    int bin_to_dec_signed(string binary_num);
    char * get_real_address(int virtual_address);

    string bin_str = dec_to_bin(*pc_real_addr());
    string rs_code = bin_str.substr(6,5);
    string rt_code = bin_str.substr(11,5);
    string rd_code = bin_str.substr(16,5);
    string sa_code = bin_str.substr(21,5);
    string immediate = bin_str.substr(16,16);

    if(bin_str.substr(0,6) == "000000"){
        //OPCODE = "000000", R-Type
        if(bin_str.substr(26,6) == "100000"){
            //R-Type, add
            *str_to_reg(rd_code) = *str_to_reg(rs_code) + *str_to_reg(rt_code);
        }
        else if(bin_str.substr(26,6) == "100001"){
            //R-Type, addu
            *str_to_reg(rd_code) = *str_to_reg(rs_code) + *str_to_reg(rt_code);
        }
        else if(bin_str.substr(26,6) == "100100"){
            //R-Type, and
            *str_to_reg(rd_code) = *str_to_reg(rs_code) & *str_to_reg(rt_code);
        }
        else if(bin_str.substr(26,6) == "011010"){
            //R-Type, div
            *$lo = *str_to_reg(rs_code) / *str_to_reg(rt_code);
            *$hi = *str_to_reg(rs_code) % *str_to_reg(rt_code);
        }
        else if(bin_str.substr(26,6) == "011011"){
            //R-Type, divu
            *$lo = *str_to_reg(rs_code) / *str_to_reg(rt_code);
            *$hi = *str_to_reg(rs_code) % *str_to_reg(rt_code);
        }
        else if(bin_str.substr(26,6) == "001001"){
            //R-Type, jalr
            *str_to_reg(rd_code) = *$pc;
            *$pc = *str_to_reg(rs_code) - 4;
        }
        else if(bin_str.substr(26,6) == "001000"){
            //R-Type, jr
            *$pc = *str_to_reg(rs_code) - 4;
        }
        else if(bin_str.substr(26,6) == "010000"){
            //R-Type, mfhi
            *str_to_reg(rd_code) = *$hi;
        }
        else if(bin_str.substr(26,6) == "010010"){
            //R-Type, mflo
            *str_to_reg(rd_code) = *$lo;
        }
        else if(bin_str.substr(26,6) == "010001"){
            //R-Type, mthi
            *$hi = *str_to_reg(rs_code);
        }
        else if(bin_str.substr(26,6) == "010011"){
            //R-Type, mtlo
            *$lo = *str_to_reg(rs_code);
        }
        else if(bin_str.substr(26,6) == "011000"){
            //R-Type, mult
            long long multiplier_one = *str_to_reg(rs_code);
            long long multiplier_two = *str_to_reg(rt_code);
            long long result_mult = multiplier_one * multiplier_two;
            *$lo = (int) result_mult;
            result_mult >>= 32;
            *$hi = (int) result_mult;
        }
        else if(bin_str.substr(26,6) == "011001"){
            //R-Type, multu
            unsigned long long multiplier_one = *str_to_reg(rs_code);
            unsigned long long multiplier_two = *str_to_reg(rt_code);
            unsigned long long result_mult = multiplier_one * multiplier_two;
            *$lo = (int) result_mult;
            result_mult >>= 32;
            *$hi = (int) result_mult;
        }
        else if(bin_str.substr(26,6) == "100111"){
            //R-Type, nor
            *str_to_reg(rd_code) = ~(*str_to_reg(rs_code) | (*str_to_reg(rt_code)));
        }
        else if(bin_str.substr(26,6) == "100101"){
            //R-Type, or
            *str_to_reg(rd_code) = (*str_to_reg(rs_code) | (*str_to_reg(rt_code)));
        }
        else if(bin_str.substr(26,6) == "000000"){
            //R-Type, sll
            *str_to_reg(rd_code) = (*str_to_reg(rt_code) << bin_to_dec(sa_code));
        }
        else if(bin_str.substr(26,6) == "000100"){
            //R-Type, sllv
            *str_to_reg(rd_code) = (*str_to_reg(rt_code) << *str_to_reg(rs_code));
        }
        else if(bin_str.substr(26,6) == "101010"){
            //R-Type, slt
            *str_to_reg(rd_code) = (*str_to_reg(rs_code) < *str_to_reg(rt_code));
        }
        else if(bin_str.substr(26,6) == "101011"){
            //R-Type, sltu
            *str_to_reg(rd_code) = ((unsigned int)*str_to_reg(rs_code) < (unsigned int)*str_to_reg(rt_code));
        }
        else if(bin_str.substr(26,6) == "000011"){
            //R-Type, sra
            *str_to_reg(rd_code) = (*str_to_reg(rt_code) >> bin_to_dec(sa_code));
        }
        else if(bin_str.substr(26,6) == "000111"){
            //R-Type, srav
            *str_to_reg(rd_code) = (*str_to_reg(rt_code) >> *str_to_reg(rs_code));
        }
        else if(bin_str.substr(26,6) == "000010"){
            //R-Type, srl
            unsigned shift_bits = *str_to_reg(rt_code);
            *str_to_reg(rd_code) = shift_bits >> bin_to_dec(sa_code);
        }
        else if(bin_str.substr(26,6) == "000110"){
            //R-Type, srlv
            unsigned shift_bits = *str_to_reg(rt_code);
            *str_to_reg(rd_code) = shift_bits >> *str_to_reg(rs_code);
        }
        else if(bin_str.substr(26,6) == "100010"){
            //R-Type, sub
            *str_to_reg(rd_code) = *str_to_reg(rs_code) - *str_to_reg(rt_code);
        }
        else if(bin_str.substr(26,6) == "100011"){
            //R-Type, subu
            *str_to_reg(rd_code) = *str_to_reg(rs_code) - *str_to_reg(rt_code);
        }

        else if(bin_str.substr(26,6) == "001100"){
            //R-Type, syscall

            if(*$v0 == 1){
                //print_int
                write_out << *$a0;
                write_out.flush();
            }
            else if(*$v0 == 4){
                //print_string
                char * printer = (char*) &*memory_pointer + *$a0 - 0x400000;
                while(*printer != '\0'){
                    write_out << *printer;
                    printer = &*printer + 1;
                }
                //write_out << '\0';
                write_out.flush();
            }
            else if(*$v0 == 5){
                //read_int
                string temp;
                getline(read_in,temp);
                *$v0 = atoi(temp.c_str());
            }
            else if(*$v0 == 8){
                //read_string
                string temp;
                getline(read_in,temp);
                char * reader = (char*) &*memory_pointer + *$a0 - 0x400000;
                for(int i = 0; i < *$a1; i++){
                    * reader = temp[i];
                    reader = &*reader + 1;
                }
                * reader = '\0';
            }
            else if(*$v0 == 9){
                //sbrk
                long long dynamicdata = (long long) &*dynamic_data;
                long long starting_pt = (long long) &*memory_pointer;
                *$v0 = (int)(dynamicdata-starting_pt + 0x400000);
                dynamic_data = &*dynamic_data + *$a0;
            }
            else if(*$v0 == 10){
                //exit
                cout<<"Normal exit with code 0" << endl;
                flag = 1;
            }
            else if(*$v0 == 11){
                //print_char
                write_out << (char) *$a0;
                write_out.flush();
            }
            else if(*$v0 == 12){
                //read_char
                string temp;
                getline(read_in,temp);
                for(int i = 0; i < temp.size(); i++){
                    if(temp[i] != ' '){
                        *$v0 = (char) temp[i];
                        break;
                    }
                }
                
            }
            else if(*$v0 == 13){
                //open

                int fd = 1;
                char * printer = (char*) &*memory_pointer + *$a0 - 0x400000;
                fd = open(printer,O_CREAT | O_RDWR , S_IRWXU);
                *$a0 = fd; 

                if(fd == -1){
                    cout<<"Cannot open that file."<<endl;
                }
                else{
                    cout<<"File open success, fd = "<<fd<<endl;
                }

            }
            else if(*$v0 == 14){
                //read
                char * printer = (char*) &*memory_pointer + *$a1 - 0x400000;
                int ret = -1;

                ret = read(*$a0,printer,*$a2);
                if(ret < 0){
                    cout<<"Read Error"<<endl;
                }
                else{
                    cout<<"Read Success, ret = "<<ret<<endl;
                }
                *$a0 = ret;


            }
            else if(*$v0 == 15){
                //write,这里的a0是file descriptor，a1才是source
                char * printer = (char*) &*memory_pointer + *$a1 - 0x400000;
                int ret = -1;
                ret = write(*$a0,printer,*$a2);
                lseek(*$a0, 0, SEEK_SET);
                
                if(ret == -1){
                    cout<<"Read Error"<<endl;
                }
                else{
                    cout<<"Read Success, ret = "<<ret<<endl;
                }
                *$a0 = ret;
            }
            else if(*$v0 == 16){
                //close
                close(*$a0);
            }
            else if(*$v0 == 17){
                //exit2，暂时不清楚和exit1的区别
                cout<<"Normal exit with code" << *$a0 << endl;
                flag = 1;
            }

        }
        else if(bin_str.substr(26,6) == "100110"){
            //R-Type, xor
            *str_to_reg(rd_code) = *str_to_reg(rs_code) ^ *str_to_reg(rt_code);
        }
    }

    else if(bin_str.substr(0,6) == "001000"){
        //I-Type, addi
        *str_to_reg(rt_code) = *str_to_reg(rs_code) + bin_to_dec_signed(immediate);
    }
    else if(bin_str.substr(0,6) == "001001"){
        //I-Type, addiu
        *str_to_reg(rt_code) = *str_to_reg(rs_code) + bin_to_dec_signed(immediate);
    }
    else if(bin_str.substr(0,6) == "001100"){
        //I-Type, andi
        *str_to_reg(rt_code) = *str_to_reg(rs_code) & bin_to_dec_signed(immediate);
    }
    else if(bin_str.substr(0,6) == "000100"){
        //I-Type, beq
        if(*str_to_reg(rs_code) == *str_to_reg(rt_code)) *$pc += 4*bin_to_dec_signed(immediate);
    }
    else if(bin_str.substr(0,6) == "000001"){
        //I-Type, bgez or bltz
        if(rt_code == "00001"){
            if(*str_to_reg(rs_code) >= 0) *$pc += 4*bin_to_dec_signed(immediate);
        }
        else if(rt_code == "00000"){
            if(*str_to_reg(rs_code) < 0) *$pc += 4*bin_to_dec_signed(immediate);
        }
    }
    else if(bin_str.substr(0,6) == "000111"){
        //I-Type, bgtz
        if(*str_to_reg(rs_code) > 0) *$pc += 4*bin_to_dec_signed(immediate);
    }
    else if(bin_str.substr(0,6) == "000110"){
        //I-Type, blez
        if(*str_to_reg(rs_code) <= 0) *$pc += 4*bin_to_dec_signed(immediate);
    }
    else if(bin_str.substr(0,6) == "000101"){
        //I-Type, bne
        if(*str_to_reg(rs_code) != *str_to_reg(rt_code)) *$pc += 4*bin_to_dec_signed(immediate);
    }
    else if(bin_str.substr(0,6) == "100000"){
        //I-Type, lb
        int8_t * target;
        target = (int8_t *)get_real_address(*str_to_reg(rs_code)) + bin_to_dec_signed(immediate);
        *str_to_reg(rt_code) = *target;
    }
    else if(bin_str.substr(0,6) == "100100"){
        //I-Type, lbu
        uint8_t * target;
        target = (uint8_t *)get_real_address(*str_to_reg(rs_code)) + bin_to_dec_signed(immediate);
        *str_to_reg(rt_code) = *target;
    }
    else if(bin_str.substr(0,6) == "100001"){
        //I-Type, lh
        int16_t * target;
        int8_t * address =  (int8_t*) get_real_address(*str_to_reg(rs_code));
        target = (int16_t *) (address + bin_to_dec_signed(immediate));
        *str_to_reg(rt_code) = *target;
    }
    else if(bin_str.substr(0,6) == "100101"){
        //I-Type, lhu
        uint16_t * target;
        uint8_t * address =  (uint8_t*) get_real_address(*str_to_reg(rs_code));
        target = (uint16_t *) (address + bin_to_dec_signed(immediate));
        *str_to_reg(rt_code) = *target;
    }
    else if(bin_str.substr(0,6) == "001111"){
        //I-Type, lui
        int lower_bit = bin_to_dec_signed(immediate) & 0xffff;
        lower_bit <<= 16;
        *str_to_reg(rt_code) = lower_bit;
    }
    else if(bin_str.substr(0,6) == "100011"){
        //I-Type, lw
        int * target;
        int8_t * address =  (int8_t*) get_real_address(*str_to_reg(rs_code));
        target = (int *) (address + bin_to_dec_signed(immediate));
        *str_to_reg(rt_code) = *target;
    }
    else if(bin_str.substr(0,6) == "001101"){
        //I-Type, ori
        *str_to_reg(rt_code) = *str_to_reg(rs_code) | bin_to_dec_signed(immediate);
    }
    else if(bin_str.substr(0,6) == "101000"){
        //I-Type, sb
        int8_t lower_byte = int8_t (*str_to_reg(rt_code));
        int8_t * target;
        int8_t * address =  (int8_t*) get_real_address(*str_to_reg(rs_code));
        target = (int8_t *) (address + bin_to_dec_signed(immediate));
        *target = lower_byte;
    }
    else if(bin_str.substr(0,6) == "001010"){
        //I-Type, slti
        *str_to_reg(rt_code) = ( *str_to_reg(rs_code) < bin_to_dec_signed(immediate));
    }
    else if(bin_str.substr(0,6) == "001011"){
        //I-Type, sltiu
        *str_to_reg(rt_code) = ((unsigned int) *str_to_reg(rs_code) < (unsigned int) bin_to_dec_signed(immediate));
    }
    else if(bin_str.substr(0,6) == "101001"){
        //I-Type, sh
        int16_t lower_byte = int16_t (*str_to_reg(rt_code));
        int16_t * target;
        int8_t * address =  (int8_t*) get_real_address(*str_to_reg(rs_code));
        target = (int16_t *) (address + bin_to_dec_signed(immediate));
        *target = lower_byte;
    }
    else if(bin_str.substr(0,6) == "101011"){
        //I-Type, sw
        int lower_byte = int (*str_to_reg(rt_code));
        int * target;
        int8_t * address =  (int8_t*) get_real_address(*str_to_reg(rs_code));
        target = (int *) (address + bin_to_dec_signed(immediate));
        *target = lower_byte;
    }
    else if(bin_str.substr(0,6) == "001110"){
        //I-Type, xori
        *str_to_reg(rt_code) = *str_to_reg(rs_code) ^ bin_to_dec_signed(immediate);
    }
    else if(bin_str.substr(0,6) == "100010"){
        //I-Type, lwl
        int shift_bits = bin_to_dec_signed(immediate) ;
        char * reg_pt = (char*) str_to_reg(rt_code);
        char * mem_pt = (char*) get_real_address(*str_to_reg(rs_code)) + shift_bits;
        for(int i = 0; i<=3 - shift_bits%4; i++){
            *reg_pt = *mem_pt;
            reg_pt = &*reg_pt+1;
            mem_pt = &*mem_pt+1;
        }
    }
    else if(bin_str.substr(0,6) == "100110"){
        //I-Type, lwr
        int shift_bits = bin_to_dec_signed(immediate);
        char * reg_pt = (char*) str_to_reg(rt_code) + 3;
        char * mem_pt = get_real_address(*str_to_reg(rs_code)) + shift_bits;
        for(int i = 0; i<=(shift_bits%4); i++){
            *reg_pt = *mem_pt;
            reg_pt = &*reg_pt-1;
            mem_pt = &*mem_pt-1;
        }
    }
    else if(bin_str.substr(0,6) == "101010"){
        //I-Type, swl
        int shift_bits = bin_to_dec_signed(immediate) ;
        char * reg_pt = (char*) str_to_reg(rt_code);
        char * mem_pt = (char*) get_real_address(*str_to_reg(rs_code)) + shift_bits;
        for(int i = 0; i<=3 - shift_bits%4; i++){
            *mem_pt = *reg_pt;
            reg_pt = &*reg_pt+1;
            mem_pt = &*mem_pt+1;
        }
    }
    else if(bin_str.substr(0,6) == "101110"){
        //I-Type, swr
        int shift_bits = bin_to_dec_signed(immediate);
        char * reg_pt = (char*) str_to_reg(rt_code) +3;
        char * mem_pt = (char*) get_real_address(*str_to_reg(rs_code)) + shift_bits;
        for(int i = 0; i<=(shift_bits%4); i++){
            *mem_pt = *reg_pt;
            reg_pt = &*reg_pt-1;
            mem_pt = &*mem_pt-1;
        }
    }
    else if(bin_str.substr(0,6) == "000010"){
        //J-Type, j
        string target = bin_str.substr(6,26);
        *$pc = 4*(bin_to_dec(target))-4;
    }
    else if(bin_str.substr(0,6) == "000011"){
        //j-Type, jal
        string target = bin_str.substr(6,26);
        *$ra = *$pc + 4;
        *$pc = 4*(bin_to_dec(target))-4;
    }
    
    *$pc += 4;      //In normal cases, pc jumps to next line.
}



//Return pc's real address
int * pc_real_addr(){
    return (int*) &*memory_pointer + (*$pc-0x400000)/4;
}

char * get_real_address(int virtual_address){
    return (char*) (&*memory_pointer) + virtual_address - 0x400000;
}

int * str_to_reg(string bin_codes){
    if(bin_codes == "00000") return &*$zero;
    else if(bin_codes == "00001") return &*$at;
    else if(bin_codes == "00010") return &*$v0;
    else if(bin_codes == "00011") return &*$v1;
    else if(bin_codes == "00100") return &*$a0;
    else if(bin_codes == "00101") return &*$a1;
    else if(bin_codes == "00110") return &*$a2;
    else if(bin_codes == "00111") return &*$a3;
    else if(bin_codes == "01000") return &*$t0;
    else if(bin_codes == "01001") return &*$t1;
    else if(bin_codes == "01010") return &*$t2;
    else if(bin_codes == "01011") return &*$t3;
    else if(bin_codes == "01100") return &*$t4;
    else if(bin_codes == "01101") return &*$t5;
    else if(bin_codes == "01110") return &*$t6;
    else if(bin_codes == "01111") return &*$t7;
    else if(bin_codes == "10000") return &*$s0;
    else if(bin_codes == "10001") return &*$s1;
    else if(bin_codes == "10010") return &*$s2;
    else if(bin_codes == "10011") return &*$s3;
    else if(bin_codes == "10100") return &*$s4;
    else if(bin_codes == "10101") return &*$s5;
    else if(bin_codes == "10110") return &*$s6;
    else if(bin_codes == "10111") return &*$s7;
    else if(bin_codes == "11000") return &*$t8;
    else if(bin_codes == "11001") return &*$t9;
    else if(bin_codes == "11010") return &*$k0;
    else if(bin_codes == "11011") return &*$k1;
    else if(bin_codes == "11100") return &*$gp;
    else if(bin_codes == "11101") return &*$sp;
    else if(bin_codes == "11110") return &*$fp;
    else if(bin_codes == "11111") return &*$ra;
    else return nullptr;
}

int main(int argc, char *argv[]){
    
    if(argc != 6){
        cout<<"Please input valid arguments, like'./simulator test.asm test.txt test_checkpts.txt test.in test.out' "<<endl;
        return 0;
    }
    

    asm_file.open(argv[1]);
    if(!asm_file.is_open()){
        cout<<"Cannot open input file."<<endl;
        return 0;
    }
   
    txt_file.open(argv[2]);
    if(!txt_file.is_open()){
        cout<<"Cannot open input file."<<endl;
        return 0;
    }

    checkpoint.open(argv[3]);
    if(!checkpoint.is_open()){
        cout<<"Cannot open input file."<<endl;
        return 0;
    }

    read_in.open(argv[4]);
    if(!read_in.is_open()){
        cout<<"Cannot open input file."<<endl;
        return 0;
    }

    write_out.open(argv[5]);
    if(!write_out.is_open()){
        cout<<"Cannot open input file."<<endl;
        return 0;
    }

    
   //Initialize virtual memory, pointers etc.
    memory_pointer = (int*) malloc(0x600000);
    register_baseline = (int*) malloc(140);

    $zero = &*register_baseline;
    $at = &*register_baseline+1;
    $v0 = &*register_baseline+2;
    $v1 = &*register_baseline+3;
    $a0 = &*register_baseline+4;
    $a1 = &*register_baseline+5;
    $a2 = &*register_baseline+6;
    $a3 = &*register_baseline+7;
    $t0 = &*register_baseline+8;
    $t1 = &*register_baseline+9;
    $t2 = &*register_baseline+10;
    $t3 = &*register_baseline+11;
    $t4 = &*register_baseline+12;
    $t5 = &*register_baseline+13;
    $t6 = &*register_baseline+14;
    $t7 = &*register_baseline+15;
    $s0 = &*register_baseline+16;
    $s1 = &*register_baseline+17;
    $s2 = &*register_baseline+18;
    $s3 = &*register_baseline+19;
    $s4 = &*register_baseline+20;
    $s5 = &*register_baseline+21;
    $s6 = &*register_baseline+22;
    $s7 = &*register_baseline+23;
    $t8 = &*register_baseline+24;
    $t9 = &*register_baseline+25;
    $k0 = &*register_baseline+26;
    $k1 = &*register_baseline+27;
    $gp = &*register_baseline+28;
    $sp = &*register_baseline+29;
    $fp = &*register_baseline+30;
    $ra = &*register_baseline+31;
    $pc = &*register_baseline+32;
    $hi = &*register_baseline+33;
    $lo = &*register_baseline+34;

    //Ways to modify registers:

    *$gp = 0x508000;   //For here, 0x508000 is the address in MIPS, and the simulated 6MB starts at 0x400000;
    *$fp = 0xA00000;
    *$sp = 0xA00000;
    *$pc = 0x400000;




    init_checkpoints();
    text_segment_init();
    static_data_init();
    
    while(true){
        checkpoint_memory(ins_count);
        checkpoint_register(ins_count);

        if(flag == 1) break;
        execute();
        ins_count++;        
    }

    asm_file.close();
    txt_file.close();
    read_in.close();
    write_out.close();
    
    return 0;


}