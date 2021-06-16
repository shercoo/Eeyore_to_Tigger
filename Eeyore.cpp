//
// Created by sherco on 2021/6/8.
//
#include "Enode.h"
#include "EsymTable.h"
#include <iostream>
#include <sstream>
using namespace std;

EsymTable* currentSymTable=new EsymTable();
stringstream fcode;
bool inFunc;
int stackTop=0;
int paramCnt=0;
Symbol* lastSymbol;
Symbol* numSymbol=new Symbol("",Symbol::Num);
#define reg currentSymTable->regs
#define regName(x) reg[x]->regName

void loadVal(Symbol* sb,int regId){
    if(sb->type==Symbol::Global){
        if(!sb->isArr)
            fcode<<"load "<<sb->name<<" "<<regName(regId)<<endl;
        else
            fcode<<"loadaddr "<<sb->name<<" "<<regName(regId)<<endl;
    }
    else if(sb->type==Symbol::Local){
        if(!sb->isArr) {
            if (sb->stored)
                fcode << "load " << sb->stackPos << " " << regName(regId) << endl;
            else
                fcode << regName(regId) << " = 0" << endl;
        }
        else
            fcode<<"loadaddr "<<sb->stackPos<<" "<<regName(regId)<<endl;
    }
    else if(sb->type==Symbol::Param){
        fcode << "load " << sb->stackPos << " " << regName(regId) << endl;
    }
    else if(sb->type==Symbol::Num){
        fcode<<regName(regId)<<" = "<<sb->name<<endl;
    }
    sb->regId=regId;
}

void storeVal(Symbol* sb,int regId){
    if(sb->type==Symbol::Global){
        fcode<<"loadaddr "<<sb->name<<" "<<regName(3)<<endl;
        fcode<<regName(3)<<" [ 0 ] = "<<regName(regId)<<endl;
    }
    else if(sb->type==Symbol::Local
            ||sb->type==Symbol::Param){
        fcode<<"store "<<regName(regId)<<" "<<sb->stackPos<<endl;
        sb->stored= true;
    }
}


void start(Enode* root){
    root->process();
}

void Program::process() {
    if(declaration!= nullptr)
        declaration->process();
    else if(functionDef!= nullptr)
        functionDef->process();
    if(program1!= nullptr)
        program1->process();
}

void Declaration::process() {
    Symbol* symbol;
    if(!inFunc) {
        if (size <= 0) {
            symbol= currentSymTable->addSymbol(ident, Symbol::Global);
            symbol->isArr= false;
            cout << symbol->name << " = 0" << endl;
        } else {
            symbol= currentSymTable->addSymbol(ident, Symbol::Global);
            symbol->isArr= true;
            cout << symbol->name << " = malloc " << size << endl;
        }
    }
    else{
        if (size <= 0) {
            symbol=currentSymTable->addSymbol(ident, Symbol::Local);
            symbol->isArr= false;
            symbol->stackPos=stackTop;
            stackTop++;
        } else {
            symbol=currentSymTable->addSymbol(ident, Symbol::Local);
            symbol->isArr= true;
            symbol->stackPos=stackTop;
            stackTop+=size/4;
        }
    }
}

void FunctionDef::process() {
    inFunc= true;
    stackTop=args;
    for(int i=0;i<args;i++) {
        Symbol *symbol = currentSymTable->addSymbol("p"+ to_string(i), Symbol::Param);
        symbol->isArr= false;
        symbol->stackPos = i;
        storeVal(symbol,i+12);
    }
    statement->process();
    cout<<fname<<" ["<<args<<"] ["<<stackTop<<"]"<<endl;
    cout<<fcode.str();
    fcode.str("");
    cout<<"end "<<fname<<endl<<endl;
}

void Statement::process() {
//    cout<<fcode.str()<<endl;
//    fcode.str("");
    if(expression!= nullptr){
        expression->process();
    }
    else if(declaration!= nullptr)
        declaration->process();
    if(statement1!= nullptr)
        statement1->process();
}


void Expression::process() {
    Symbol* sb0;
    Symbol* sb1;
    Symbol* sb2;
    switch (type) {

        case(Expression::Binary):
            rightvalue1->process();
            sb0=lastSymbol;
            rightvalue2->process();
            sb1=lastSymbol;
            loadVal(sb0,0);
            loadVal(sb1,1);
            fcode<<regName(2)<<" = "<<regName(0)<<" "<<op<<" "<<regName(1)<<endl;
            sb2=currentSymTable->getSymbol(ident1);
            storeVal(sb2,2);
            break;

        case(Expression::Unary):
            rightvalue1->process();
            sb0=lastSymbol;
            loadVal(sb0,0);
            fcode<<regName(2)<<" = "<<op<<" "<<regName(0)<<endl;
            sb2=currentSymTable->getSymbol(ident1);
            storeVal(sb2,2);
            break;

        case(Expression::Assign):
            rightvalue1->process();
            sb0=lastSymbol;
            loadVal(sb0,0);
            sb2=currentSymTable->getSymbol(ident1);
            storeVal(sb2,0);
            break;

        case(Expression::LArrAssign):
            rightvalue1->process();
            sb0=lastSymbol;
            rightvalue2->process();
            sb1=lastSymbol;
            loadVal(sb0,0);
            loadVal(sb1,1);
            sb2=currentSymTable->getSymbol(ident1);
            if(sb2->type==Symbol::Global)
                fcode<<"loadaddr "<<sb2->name<<" "<<regName(2)<<endl;
            else if(sb2->type==Symbol::Local)
                fcode<<"loadaddr "<<sb2->stackPos<<" "<<regName(2)<<endl;
            else if(sb2->type==Symbol::Param)
                fcode<<"load "<<sb2->stackPos<<" "<<regName(2)<<endl;
            fcode<<regName(2)<<" = "<<regName(2)<<" + "<<regName(0)<<endl;
            fcode<<regName(2)<<" [ 0 ] = "<<regName(1)<<endl;
            break;

        case(Expression::RArrAssign):
            sb0=currentSymTable->getSymbol(ident2);
            rightvalue1->process();
            sb1=lastSymbol;
            loadVal(sb1,0);
            if(sb0->type==Symbol::Global)
                fcode<<"loadaddr "<<sb0->name<<" "<<regName(1)<<endl;
            else if(sb0->type==Symbol::Local)
                fcode<<"loadaddr "<<sb0->stackPos<<" "<<regName(1)<<endl;
            else if(sb0->type==Symbol::Param)
                fcode<<"load "<<sb0->stackPos<<" "<<regName(1)<<endl;
            fcode<<regName(1)<<" = "<<regName(1)<<" + "<<regName(0)<<endl;
            sb2=currentSymTable->getSymbol(ident1);
            fcode<<regName(2)<<" = "<<regName(1)<<" [ 0 ]"<<endl;
            storeVal(sb2,2);
            break;

        case(Expression::If):
            rightvalue1->process();
            sb0=lastSymbol;
            rightvalue2->process();
            sb1=lastSymbol;
            loadVal(sb0,0);
            loadVal(sb1,1);
            fcode<<"if "<<regName(0)<<" "<<op<<" "<<regName(1)<<" goto "<<label<<endl;
            break;

        case(Expression::Goto):
            fcode<<"goto "<<label<<endl;
            break;

        case(Expression::Label):
            fcode<<label<<":"<<endl;
            break;

        case(Expression::Param):
            rightvalue1->process();
            sb0=lastSymbol;
            loadVal(sb0,paramCnt+12);
            paramCnt++;
            break;

        case(Expression::Call):
            fcode<<"call "<<fname<<endl;
            paramCnt=0;
            break;

        case(Expression::AssignCall):
            fcode<<"call "<<fname<<endl;
            paramCnt=0;
            sb2=currentSymTable->getSymbol(ident1);
            storeVal(sb2,12);
            break;

        case(Expression::ReturnInt):
            rightvalue1->process();
            sb0=lastSymbol;
            loadVal(sb0,paramCnt+12);
            fcode<<"return"<<endl;
            break;

        case(Expression::ReturnVoid):
            fcode<<"return"<<endl;
            break;

    }
    fcode<<endl;
}

void RightValue::process() {
    if(!ident.empty()){
        lastSymbol=currentSymTable->getSymbol(ident);
    }
    else{
        lastSymbol=new Symbol(to_string(num),Symbol::Num);
    }
}