#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;
#include "phase1.h"

VectorMap  addline(vector<string>  in);
vector<string> toBinaryCode(VectorMap in);


int main(int argc, char *argv[]){
    vector<string> original;
    vector<string> comparison;
    VectorMap after_discard;
    vector<string> machine_code;
    string tempStorage;

    if(argc != 4){
        cout<<"Please input valid arguments, like'./tester input.asm output.txt expectedoutput.txt' "<<endl;
        return 0;
    }

    ifstream testin;
    ofstream writeout;
    ifstream expectedout;

    testin.open(argv[1]);
    if(!testin.is_open()){
        cout<<"Cannot open input file."<<endl;
        return 0;
    }

    writeout.open(argv[2]);
    if(!writeout.is_open()){
        cout<<"Cannot open output file."<<endl;
        return 0;
    }

    expectedout.open(argv[3]);
    if(!expectedout.is_open()){
        cout<<"Cannot open expectedOut file."<<endl;
        return 0;
    }
    while(getline(testin,tempStorage)){
        original.push_back(tempStorage);
    }
    after_discard = addline(original);

    //将expectedout.txt导入vector comparison中
    while(getline(expectedout,tempStorage)){
        comparison.push_back(tempStorage);
    }


    machine_code = toBinaryCode(after_discard);

    for(int i = 0; i<machine_code.size(); i++){
        writeout<<machine_code[i]<<endl;
    }

    //将expectedput和output比较。
    if(machine_code == comparison) cout<<"Same!"<<endl;
    else cout<<"Oops, different."<<endl;

    testin.close();
    writeout.close();
    expectedout.close();
    
    return 0;
}