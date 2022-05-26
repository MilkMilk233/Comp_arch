#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include "phase1.h"
using namespace std;


VectorMap  addline(vector<string> in){
    int relativeAddress = 0;
    vector<string> output;
    bool isData = false;
    string finalline;
    string lineWithoutComment;      
    string::size_type position;     
    string::size_type datapos;  
    string label;
    map<string, int> labelMap;
    int indexLabel;
    bool isLabel = false;
    for(int j = 0; j < in.size(); j++){
        //消灭.data部分
        if((datapos = in[j].find(".data")) != in[j].npos) isData = true;
        else if((datapos = in[j].find(".text")) != in[j].npos){
            isData = false;
            continue;
        }
        if(isData) continue;
        //查找并删除句子中的注释         
        if((position = in[j].find("#")) != in[j].npos){
            lineWithoutComment = in[j].substr(0,position);
        }
        else lineWithoutComment = in[j];
        //识别label行，整行去除或只留下label后半部分
        if((position = lineWithoutComment.find(":")) != lineWithoutComment.npos){
            for(int k = 0; k < position; k++){      //识别第一个非空字符,k即其index
                if(lineWithoutComment[k] != ' '){
                    indexLabel = k;
                    break;
                }
            }
            label = lineWithoutComment.substr(indexLabel,position-indexLabel);   //此刻识别出了label的string，准备和LabelTable配对。
            isLabel = true;
            //TODO:和labeltable配对

            bool isendwithcolumn = true;
            for(int lst = lineWithoutComment.size()-1; lst >=0; lst-- ){    //逆向搜索，查找是否以：号结尾
                if(lineWithoutComment[lst] != ' '){
                    if(lineWithoutComment[lst] != ':'){
                        isendwithcolumn = false;
                        finalline = lineWithoutComment.substr(position+1, lineWithoutComment.size()-position-1);
                        break;
                    } 
                    
                }
            }
            if(isendwithcolumn) continue; 
        }
        else finalline = lineWithoutComment;
        //空行检测机制
        bool isEmptyLine = true;
        for(int i = 0; i < finalline.size(); i++){
            if(finalline[i] != ' '){
                isEmptyLine = false;
                break;
            }
        }
        if(isEmptyLine) continue;

        //最后，把这一行写入结果vector。
        output.push_back(finalline);
        //若前面为label，则把这一行加入map中。
        if(isLabel){
            //TODO:配对传入labelTable。
            labelMap[label] = relativeAddress;
            isLabel = false;
        }
        relativeAddress++;

    }

    VectorMap output_vectormap;
    output_vectormap.allLines = output;
    output_vectormap.labelMatch = labelMap;

    return output_vectormap;
}


    