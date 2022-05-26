#include <cstdio>
#include <iostream>
#include <vector>
#include "phase1.h"
#include <string>
#include <sstream>
using namespace std;

const map<string, string> registers = {{"$ze","00000"},{"$at","00001"},{"$v0","00010"},{"$v1","00011"},{"$a0","00100"}
    , {"$a1","00101"}, {"$a2", "00110"}, {"$a3", "00111"}, {"$t0","01000"}, {"$t1", "01001"}, {"$t2", "01010"},
     {"$t3", "01011"}, {"$t4", "01100"}, {"$t5", "01101"}, {"$t6", "01110"}, {"$t7", "01111"}, {"$s0", "10000"},
     {"$s1", "10001"}, {"$s2", "10010"}, {"$s3", "10011"},{"$s4", "10100"}, {"$s5", "10101"}, {"$s6", "10110"}, 
     {"$s7", "10111"}, {"$t8", "11000"}, {"$t9", "11001"}, {"$k0", "11010"}, {"$k1", "11011"}, {"$gp", "11100"},
     {"$sp", "11101"}, {"$s8", "11110"}, {"$fp", "11110"}, {"$ra", "11111"}};

string delete_spaces(string stri);

string dec_to_bin(string deciaml, int target_sign);

//R-Type
string tradd(string lineToken);
string traddu(string lineToken);
string trand(string lineToken);
string trdiv(string lineToken);
string trdivu(string lineToken);
string trjalr(string lineToken);
string trjr(string lineToken);
string trmfhi(string lineToken);
string trmflo(string lineToken);
string trmthi(string lineToken);
string trmtlo(string lineToken);
string trmult(string lineToken);
string trmultu(string lineToken);
string trnor(string lineToken);
string tror(string lineToken);
string trsll(string lineToken);
string trsllv(string lineToken);
string trslt(string lineToken);
string trsltu(string lineToken);
string trsra(string lineToken);
string trsrav(string lineToken);
string trsrl(string lineToken);
string trsrlv(string lineToken);
string trsub(string lineToken);
string trsubu(string lineToken);
string trsyscall(string lineToken);
string trxor(string lineToken);

//I-Type
string traddi(string lineToken);
string traddiu(string lineToken);
string trandi(string lineToken);
string trbeq(string lineToken,map<string, int> mapping, int current_address);
string trbgez(string lineToken, map<string, int> mapping, int current_address);
string trbgtz(string lineToken, map<string, int> mapping, int current_address);
string trblez(string lineToken, map<string, int> mapping, int current_address);
string trbltz(string lineToken, map<string, int> mapping, int current_address);
string trbne(string lineToken, map<string, int> mapping, int current_address);
string trlb(string lineToken);
string trlbu(string lineToken);
string trlh(string lineToken);
string trlhu(string lineToken);
string trlui(string lineToken);
string trlw(string lineToken);
string trori(string lineToken);
string trsb(string lineToken);
string trslti(string lineToken);
string trsltiu(string lineToken);
string trsh(string lineToken);
string trsw(string lineToken);
string trxori(string lineToken);
string trlwl(string lineToken);
string trlwr(string lineToken);
string trswl(string lineToken);
string trswr(string lineToken);

//J-Type
string trj(string lineToken, map<string,int> mapping);
string trjal(string lineToken, map<string,int> mapping);

/*
string sign_extent(string binary, int target_sign){
    stringstream model;
    for(int i = 0; i < target_sign - binary.size(); i++){
        model << binary[0];
    }
    return model.str() + binary;
}
*/

vector<string> toBinaryCode(VectorMap in){
    string lineToken;
    vector<string> allBinaryCode;
    for(int i = 0; i<in.allLines.size(); i++){
        lineToken = delete_spaces(in.allLines[i]);
        //主战场，识别各类instruction type放进不同translation函数。
        if(lineToken.find("add$") == 0) allBinaryCode.push_back(tradd(lineToken));
        else if(lineToken.find("addu$") == 0) allBinaryCode.push_back(traddu(lineToken));
        else if(lineToken.find("and$") == 0) allBinaryCode.push_back(trand(lineToken));
        else if(lineToken.find("div$") == 0) allBinaryCode.push_back(trdiv(lineToken));
        else if(lineToken.find("divu$") == 0) allBinaryCode.push_back(trdivu(lineToken));
        else if(lineToken.find("jalr$") == 0) allBinaryCode.push_back(trjalr(lineToken));
        else if(lineToken.find("jr$") == 0) allBinaryCode.push_back(trjr(lineToken));
        else if(lineToken.find("mfhi$") == 0) allBinaryCode.push_back(trmfhi(lineToken));
        else if(lineToken.find("mflo$") == 0) allBinaryCode.push_back(trmflo(lineToken));
        else if(lineToken.find("mthi$") == 0) allBinaryCode.push_back(trmthi(lineToken));
        else if(lineToken.find("mtlo$") == 0) allBinaryCode.push_back(trmtlo(lineToken));
        else if(lineToken.find("mult$") == 0) allBinaryCode.push_back(trmult(lineToken));
        else if(lineToken.find("multu$") == 0) allBinaryCode.push_back(trmultu(lineToken));
        else if(lineToken.find("nor$") == 0) allBinaryCode.push_back(trnor(lineToken));
        else if(lineToken.find("or$") == 0) allBinaryCode.push_back(tror(lineToken));
        else if(lineToken.find("sll$") == 0) allBinaryCode.push_back(trsll(lineToken));
        else if(lineToken.find("sllv$") == 0) allBinaryCode.push_back(trsllv(lineToken));
        else if(lineToken.find("slt$") == 0) allBinaryCode.push_back(trslt(lineToken));
        else if(lineToken.find("sltu$") == 0) allBinaryCode.push_back(trsltu(lineToken));
        else if(lineToken.find("sra$") == 0) allBinaryCode.push_back(trsra(lineToken));
        else if(lineToken.find("srav$") == 0) allBinaryCode.push_back(trsrav(lineToken));
        else if(lineToken.find("srl$") == 0) allBinaryCode.push_back(trsrl(lineToken));
        else if(lineToken.find("srlv$") == 0) allBinaryCode.push_back(trsrlv(lineToken));
        else if(lineToken.find("sub$") == 0) allBinaryCode.push_back(trsub(lineToken));
        else if(lineToken.find("subu$") == 0) allBinaryCode.push_back(trsubu(lineToken));
        else if(lineToken.find("xor$") == 0) allBinaryCode.push_back(trxor(lineToken));
        else if(lineToken.find("addi$") == 0) allBinaryCode.push_back(traddi(lineToken));
        else if(lineToken.find("addiu$") == 0) allBinaryCode.push_back(traddiu(lineToken));
        else if(lineToken.find("andi$") == 0) allBinaryCode.push_back(trandi(lineToken));
        else if(lineToken.find("beq$") == 0) allBinaryCode.push_back(trbeq(lineToken, in.labelMatch, i));
        else if(lineToken.find("bgez$") == 0) allBinaryCode.push_back(trbgez(lineToken, in.labelMatch, i));
        else if(lineToken.find("bgtz$") == 0) allBinaryCode.push_back(trbgtz(lineToken, in.labelMatch, i));
        else if(lineToken.find("blez$") == 0) allBinaryCode.push_back(trblez(lineToken, in.labelMatch, i));
        else if(lineToken.find("bltz$") == 0) allBinaryCode.push_back(trbltz(lineToken, in.labelMatch, i));
        else if(lineToken.find("bne$") == 0) allBinaryCode.push_back(trbne(lineToken, in.labelMatch, i));
        else if(lineToken.find("lb$") == 0) allBinaryCode.push_back(trlb(lineToken));
        else if(lineToken.find("lbu$") == 0) allBinaryCode.push_back(trlbu(lineToken));
        else if(lineToken.find("lh$") == 0) allBinaryCode.push_back(trlh(lineToken));
        else if(lineToken.find("lhu$") == 0) allBinaryCode.push_back(trlhu(lineToken));
        else if(lineToken.find("lui$") == 0) allBinaryCode.push_back(trlui(lineToken));
        else if(lineToken.find("lw$") == 0) allBinaryCode.push_back(trlw(lineToken));
        else if(lineToken.find("ori$") == 0) allBinaryCode.push_back(trori(lineToken));
        else if(lineToken.find("sb$") == 0) allBinaryCode.push_back(trsb(lineToken));
        else if(lineToken.find("slti$") == 0) allBinaryCode.push_back(trslti(lineToken));
        else if(lineToken.find("sltiu$") == 0) allBinaryCode.push_back(trsltiu(lineToken));
        else if(lineToken.find("sh$") == 0) allBinaryCode.push_back(trsh(lineToken));
        else if(lineToken.find("sw$") == 0) allBinaryCode.push_back(trsw(lineToken));
        else if(lineToken.find("xori$") == 0) allBinaryCode.push_back(trxori(lineToken));
        else if(lineToken.find("lwl$") == 0) allBinaryCode.push_back(trlwl(lineToken));
        else if(lineToken.find("lwr$") == 0) allBinaryCode.push_back(trlwr(lineToken));
        else if(lineToken.find("swl$") == 0) allBinaryCode.push_back(trswl(lineToken));
        else if(lineToken.find("swr$") == 0) allBinaryCode.push_back(trswr(lineToken));

        else if(lineToken.find("jal") == 0) allBinaryCode.push_back(trjal(lineToken, in.labelMatch));
        else if(lineToken.find("j") == 0) allBinaryCode.push_back(trj(lineToken, in.labelMatch));
        else if(lineToken.find("syscall") == 0) allBinaryCode.push_back(trsyscall(lineToken));

    }

    return allBinaryCode;
}

string dec_to_bin(string decimal, int target_sign){

    string binary;
    bool is_negative = false;
    if(decimal == "0"){
        binary = "0";
    }
    else{
        if(decimal.find("-") == 0){
            is_negative = true;
            decimal = decimal.substr(1, decimal.size()-1);
        }
        int n = (int) atoi(decimal.c_str());
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
    stringstream model;
    char addBit;
    if(is_negative) addBit = '1';
    else addBit = '0';
    for(int i = 0; i < target_sign - binary.size(); i++){
        model << addBit;
    }

    return model.str() + binary;

}


//把所有空格和tab去掉，转化为紧凑的free format.
string delete_spaces(string stri){
    stringstream lineWithoutSpace;
    for(int i = 0; i < stri.size(); i++){
        if(stri[i] != char(9) && stri[i] != ' ') lineWithoutSpace << stri[i]; 
    }
    return lineWithoutSpace.str();
}

//R-Type
string tradd(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+rd+string("00000")+string("100000");
}
string traddu(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+rd+string("00000")+string("100001");
}
string trand(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+rd+string("00000")+string("100100");
}
string trdiv(string lineToken){
    int indexof = lineToken.find('$');
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+string("00000")+string("00000")+string("011010");
}
string trdivu(string lineToken){
    int indexof = lineToken.find('$');
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+string("00000")+string("00000")+string("011011");
}
string trjalr(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+string("00000")+rd+string("00000")+string("001001");
}
string trjr(string lineToken){
    int indexof = lineToken.find('$');
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+string("00000")+string("00000")+string("00000")+string("001000");
}
string trmfhi(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+string("00000")+string("00000")+rd+string("00000")+string("010000");
}
string trmflo(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+string("00000")+string("00000")+rd+string("00000")+string("010010");
}
string trmthi(string lineToken){
    int indexof = lineToken.find('$');
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+string("00000")+string("00000")+string("00000")+string("010001");
}
string trmtlo(string lineToken){
    int indexof = lineToken.find('$');
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+string("00000")+string("00000")+string("00000")+string("010011");
}
string trmult(string lineToken){
    int indexof = lineToken.find('$');
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+string("00000")+string("00000")+string("011000");
}
string trmultu(string lineToken){
    int indexof = lineToken.find('$');
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+string("00000")+string("00000")+string("011001");
}
string trnor(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+rd+string("00000")+string("100111");
}
string tror(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+rd+string("00000")+string("100101");
}
string trsll(string lineToken){
    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;

    string sa = dec_to_bin(lineToken.substr(starting_point, lineToken.size()-starting_point),5);
    return string("000000")+string("00000")+rt+rd+sa+string("000000");
}
string trsllv(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+rd+string("00000")+string("000100");
}
string trslt(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+rd+string("00000")+string("101010");
}
string trsltu(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+rd+string("00000")+string("101011");
}
string trsra(string lineToken){
    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;

    string sa = dec_to_bin(lineToken.substr(starting_point, lineToken.size()-starting_point),5);
    return string("000000")+string("00000")+rt+rd+sa+string("000011");
}
string trsrav(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+rd+string("00000")+string("000111");
}
string trsrl(string lineToken){
    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;

    string sa = dec_to_bin(lineToken.substr(starting_point, lineToken.size()-starting_point),5);
    return string("000000")+string("00000")+rt+rd+sa+string("000010");
}
string trsrlv(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+rd+string("00000")+string("000110");
}
string trsub(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+rd+string("00000")+string("100010");
}
string trsubu(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+rd+string("00000")+string("100011");
}
string trsyscall(string lineToken){
    return string("00000000000000000000000000001100");
}
string trxor(string lineToken){
    int indexof = lineToken.find('$');
    string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    return string("000000")+rs+rt+rd+string("00000")+string("100110");
}

//I-Type
string traddi(string lineToken){
    int indexof = lineToken.find('$');
    //string rd = registers.find(lineToken.substr(indexof++,3))->second ;
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    
    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string dec_pre = lineToken.substr(starting_point, lineToken.size()-starting_point);
    string immediate = dec_to_bin(dec_pre,16);
    return string("001000")+rs+rt+immediate;
}
string traddiu(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, lineToken.size()-starting_point),16);
    return string("001001")+rs+rt+immediate;
}
string trandi(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, lineToken.size()-starting_point),16);
    return string("001100")+rs+rt+immediate;
}
string trbeq(string lineToken, map<string, int> mapping, int current_address){
    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    int indexof = lineToken.find('$');
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;

    stringstream byte_convertion;
    string str_result;
    byte_convertion << mapping[lineToken.substr(starting_point, lineToken.size()-starting_point)]-current_address-1;
    byte_convertion >> str_result;
    string label =dec_to_bin(str_result, 16);
    return string("000100")+rs+rt+ label;
}
string trbgez(string lineToken, map<string, int> mapping, int current_address){
    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    int indexof = lineToken.find('$');
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    string rt = "00001";

    stringstream byte_convertion;
    string str_result;
    byte_convertion << mapping[lineToken.substr(starting_point, lineToken.size()-starting_point)]-current_address-1;
    byte_convertion >> str_result;
    string label =dec_to_bin(str_result, 16);
    return string("000001")+rs+rt+ label;
}
string trbgtz(string lineToken, map<string, int> mapping, int current_address){
    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    int indexof = lineToken.find('$');
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    string rt = "00000";

    stringstream byte_convertion;
    string str_result;
    byte_convertion << mapping[lineToken.substr(starting_point, lineToken.size()-starting_point)]-current_address-1;
    byte_convertion >> str_result;
    string label =dec_to_bin(str_result, 16);
    return string("000111")+rs+rt+ label;
}
string trblez(string lineToken, map<string, int> mapping, int current_address){
    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    int indexof = lineToken.find('$');
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    string rt = "00000";

    stringstream byte_convertion;
    string str_result;
    byte_convertion << mapping[lineToken.substr(starting_point, lineToken.size()-starting_point)]-current_address-1;
    byte_convertion >> str_result;
    string label =dec_to_bin(str_result, 16);
    return string("000110")+rs+rt+ label;
}
string trbltz(string lineToken, map<string, int> mapping, int current_address){
    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    int indexof = lineToken.find('$');
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    string rt = "00000";

    stringstream byte_convertion;
    string str_result;
    byte_convertion << mapping[lineToken.substr(starting_point, lineToken.size()-starting_point)]-current_address-1;
    byte_convertion >> str_result;
    string label =dec_to_bin(str_result, 16);
    return string("000110")+rs+rt+ label;
}
string trbne(string lineToken, map<string, int> mapping, int current_address){
    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    int indexof = lineToken.find('$');
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;

    stringstream byte_convertion;
    string str_result;
    byte_convertion << mapping[lineToken.substr(starting_point, lineToken.size()-starting_point)]-current_address-1;
    byte_convertion >> str_result;
    string label =dec_to_bin(str_result, 16);
    return string("000101")+rs+rt+ label;
}
string trlb(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    int ending_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == '('){
            ending_point = i;
        }

        else if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, ending_point-starting_point),16);
    return string("100000")+rs+rt+immediate;
}
string trlbu(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    int ending_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == '('){
            ending_point = i;
        }

        else if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, ending_point-starting_point),16);
    return string("100100")+rs+rt+immediate;
}
string trlh(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    int ending_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == '('){
            ending_point = i;
        }

        else if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, ending_point-starting_point),16);
    return string("100001")+rs+rt+immediate;
}
string trlhu(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    int ending_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == '('){
            ending_point = i;
        }

        else if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, ending_point-starting_point),16);
    return string("100101")+rs+rt+immediate;
}
string trlui(string lineToken){
    int indexof = lineToken.find('$');
    string rs = "00000" ;
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, lineToken.size()-starting_point),16);
    return string("001111")+rs+rt+immediate;
}
string trlw(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    int ending_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == '('){
            ending_point = i;
        }

        else if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, ending_point-starting_point),16);
    return string("100011")+rs+rt+immediate;
}
string trori(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, lineToken.size()-starting_point),16);
    return string("001101")+rs+rt+immediate;
}
string trsb(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    int ending_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == '('){
            ending_point = i;
        }

        else if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, ending_point-starting_point),16);
    return string("101000")+rs+rt+immediate;
}
string trslti(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, lineToken.size()-starting_point),16);
    return string("001010")+rs+rt+immediate;
}
string trsltiu(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, lineToken.size()-starting_point),16);
    return string("001011")+rs+rt+immediate;
}
string trsh(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    int ending_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == '('){
            ending_point = i;
        }

        else if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, ending_point-starting_point),16);
    return string("101001")+rs+rt+immediate;
}
string trsw(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    int ending_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == '('){
            ending_point = i;
        }

        else if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, ending_point-starting_point),16);
    return string("101011")+rs+rt+immediate;
}
string trxori(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, lineToken.size()-starting_point),16);
    return string("001110")+rs+rt+immediate;
}
string trlwl(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    int ending_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == '('){
            ending_point = i;
        }

        else if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, ending_point-starting_point),16);
    return string("100010")+rs+rt+immediate;
}
string trlwr(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    int ending_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == '('){
            ending_point = i;
        }

        else if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, ending_point-starting_point),16);
    return string("100110")+rs+rt+immediate;
}
string trswl(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    int ending_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == '('){
            ending_point = i;
        }

        else if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, ending_point-starting_point),16);
    return string("101010")+rs+rt+immediate;
}
string trswr(string lineToken){
    int indexof = lineToken.find('$');
    string rt = registers.find(lineToken.substr(indexof++,3))->second ;
    indexof = lineToken.find('$', indexof+1);
    string rs = registers.find(lineToken.substr(indexof++,3))->second ;

    int starting_point;
    int ending_point;
    for(int i = lineToken.size()-1; i>=0; i--){
        if(lineToken[i] == '('){
            ending_point = i;
        }

        else if(lineToken[i] == ','){
            starting_point = i+1;
            break;
        }
    }
    string immediate = dec_to_bin(lineToken.substr(starting_point, ending_point-starting_point),16);
    return string("101110")+rs+rt+immediate;
}

//J-Type
string trj(string lineToken, map<string,int> mapping){
    string label = lineToken.substr(1, lineToken.size()-1);
    string str_result;
    stringstream convertion;
    convertion << (mapping[label]+1048576);
    convertion >> str_result;
    string target = dec_to_bin(str_result, 26);
    return string("000010") + target;
    
}
string trjal(string lineToken, map<string,int> mapping){
    string label = lineToken.substr(3, lineToken.size()-1);
    string str_result;
    stringstream convertion;
    convertion << (mapping[label]+1048576);
    convertion >> str_result;
    string target = dec_to_bin(str_result, 26);
    return string("000011") + target;
}

