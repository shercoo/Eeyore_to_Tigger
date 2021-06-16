//
// Created by sherco on 2021/6/15.
//

#ifndef EEYORE_TO_TIGGER_ESYMTABLE_H
#define EEYORE_TO_TIGGER_ESYMTABLE_H

#include <string>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
class Symbol;
class EsymTable;
extern EsymTable* currentSymTable;

class Register{
public:
    bool available;
    string symbolName;
    string regName;
    Register(string name):regName(name){}
};

class Symbol{
public:
    static int globalCnt;
    enum{Global,Local,Num,Param};
    string name;
    int type;
    int stackPos;
    int regId;
    bool stored;
    Symbol(string n,int t):name(n),type(t),stored(false)
    {
        if(type==Global)
            name="v"+ to_string(globalCnt++);
    }
    void loadVal(){

    }
    void storeVal(){

    }
};
int Symbol::globalCnt=0;

class EsymTable{
public:
    map<string,Symbol* > table;
    Register* regs[26];
    EsymTable(){
        for(int i=0;i<12;i++)
            regs[i]=new Register("s"+ to_string(i));
        for(int i=0;i<8;i++)
            regs[12+i]=new Register("a"+ to_string(i));
        for(int i=0;i<6;i++)
            regs[20+i]=new Register("t"+ to_string(i+2));
    }
    Symbol* addSymbol(string name,int type){
        Symbol* symbol=new Symbol(name,type);
        table[name]=symbol;
        return symbol;
    }
    Symbol* getSymbol(string name){
        return table[name];
    }
};


#endif //EEYORE_TO_TIGGER_ESYMTABLE_H
