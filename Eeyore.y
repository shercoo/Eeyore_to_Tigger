%{
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "Enode.h"
int yylex(void);
void yyerror(char*);
extern int yydebug;
struct Enode* root;
using namespace std;
%}

%union{
int iValue;
const char* sValue;
struct Enode* enode;
}

%token IF VAR RETURN GOTO CALL PARAM END
%token<iValue> NUMBER
%token<sValue> IDENT FUNC LABEL
%type<sValue> BinOP UnaryOP
%type<enode> Program Expression Declaration FunctionDef Statement RightValue
%%


Program:
 Declaration{$$=new Program(NULL,$1,NULL);}
|FunctionDef{$$=new Program(NULL,NULL,$1);}
|Program Declaration{$$=new Program($1,$2,NULL);}
|Program FunctionDef{$$=new Program($1,NULL,$2);}
|Program END{root=$1->start;
start(root);return 0;}
;

Declaration:
 VAR IDENT{$$=new Declaration($2,-1);}
|VAR NUMBER IDENT{$$=new Declaration($3,$2);}

FunctionDef:
 FUNC '[' NUMBER ']' Statement END FUNC{$$=new FunctionDef($1,$3,$5->start);}
;

Statement:
 Expression{$$=new Statement($1,NULL,NULL);}
|Declaration{$$=new Statement(NULL,$1,NULL);}
|Statement Expression{$$=new Statement($2,NULL,$1);}
|Statement Declaration{$$=new Statement(NULL,$2,$1);}
;

Expression :
 IDENT '=' RightValue BinOP RightValue{$$=new Expression($1,"",$4,"",$3,$5,"",0);}
|IDENT '=' UnaryOP RightValue{$$=new Expression($1,"",$3,"",$4,NULL,"",1);}
|IDENT '=' RightValue{$$=new Expression($1,"","","",$3,NULL,"",2);}
|IDENT '[' RightValue ']' '=' RightValue{$$=new Expression($1,"","","",$3,$6,"",3);}
|IDENT '=' IDENT '[' RightValue ']'{$$=new Expression($1,$3,"","",$5,NULL,"",4);}
|IF RightValue BinOP RightValue GOTO LABEL{$$=new Expression("","",$3,"",$2,$4,$6,5);}
|GOTO LABEL{$$=new Expression("","","","",NULL,NULL,$2,6);}
|LABEL ':'{$$=new Expression("","","","",NULL,NULL,$1,7);}
|PARAM RightValue{$$=new Expression("","","","",$2,NULL,"",8);}
|CALL FUNC{$$=new Expression("","","",$2,NULL,NULL,"",9);}
|IDENT '=' CALL FUNC{$$=new Expression($1,"","",$4,NULL,NULL,"",10);}
|RETURN RightValue{$$=new Expression("","","","",$2,NULL,"",11);}
|RETURN{$$=new Expression("","","","",NULL,NULL,"",12);}
;

RightValue:
 IDENT{$$=new RightValue($1,0);}
|NUMBER{$$=new RightValue("",$1);}
;

BinOP:
 '=' '='{$$="==";}
|'!' '='{$$="!=";}
|'='{$$="=";}
|'+'{$$="+";}
|'-'{$$="-";}
|'*'{$$="*";}
|'/'{$$="/";}
|'%'{$$="%";}
|'&' '&'{$$="&&";}
|'|' '|'{$$="||";}
|'>'{$$=">";}
|'<'{$$="<";}
|'>' '='{$$=">=";}
|'<' '='{$$="<=";}
;

UnaryOP:
 '!'{$$="!";}
|'-'{$$="-";}
;

%%

int main(int argc,char **argv)
{
        freopen(argv[3], "r", stdin);
        freopen(argv[5], "w", stdout);
        //yydebug=1;
        yyparse();
        printf("\n");
	return 0;
}
