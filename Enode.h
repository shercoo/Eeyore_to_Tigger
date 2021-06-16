//
// Created by sherco on 2021/6/14.
//

#ifndef EEYORE_TO_TIGGER_ENODE_H
#define EEYORE_TO_TIGGER_ENODE_H

#include <string>
#include <utility>
using namespace std;


class Enode{
public:
    Enode *start,*end;
    virtual void process(){}
};

void start(Enode* root);

class Program;
class Declaration;
class FunctionDef;
class Statement;
class Expression;
class RightValue;


class Program : public Enode
{
public:
    Enode* program1;
    Enode* declaration;
    Enode* functionDef;
    Program(Enode* p,Enode* d,Enode* f):declaration(d),functionDef(f){
        if(p!= nullptr) {
            Program* pp=(Program*)p;
            pp->program1= this;
            start=pp->start;
        }
        else
            start= this;
    }
    void process();
};

class Declaration:public Enode
{
public:
    string ident;
    int size;
    Declaration(const char * i,int s):ident(i),size(s){}
    void process();
};

class FunctionDef:public Enode{
public:
    string fname;
    int args;
    Enode* statement;
    FunctionDef(const char * f,int a,Enode* s):fname(f),args(a),statement(s){}
    void process();
};

class Statement:public Enode{
public:
    Enode* expression;
    Enode* declaration;
    Enode* statement1;
    Statement(Enode* e,Enode* d,Enode* s):expression(e),declaration(d){
        if(s!= nullptr){
            Statement* ss=(Statement*)s;
            ss->statement1= this;
            start=ss->start;
        }
        else
            start= this;
    }
    void process();
};

class RightValue:public Enode{
public:
    string ident;
    int num;
    RightValue(const char * i,int n):ident(i),num(n){}
    void process();
};

class Expression:public Enode{
public:
    enum{
        Binary,
        Unary,
        Assign,
        LArrAssign,
        RArrAssign,
        If,
        Goto,
        Label,
        Param,
        Call,
        AssignCall,
        ReturnInt,
        ReturnVoid
    };
    string ident1;
    string ident2;
    string op;
    string fname;
    Enode* rightvalue1;
    Enode* rightvalue2;
    string label;
    int type;
    Expression(const char * i1,const char * i2,const char * o,const char * f,Enode* r1,Enode* r2,const char * l,int t):
    ident1(i1),ident2(i2),op(o),fname(f),rightvalue1(r1),rightvalue2(r2),label(l),type(t){}
    void process();
};


#endif //EEYORE_TO_TIGGER_ENODE_H
