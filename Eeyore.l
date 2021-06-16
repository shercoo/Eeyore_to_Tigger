%{
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"Eeyore.tab.hpp"

int line=1;
%}


digit       [0-9]
number      -?{digit}+
label       l{number}
letter      [a-zA-Z_]
identifier  {letter}({letter}|{digit})*
variable    [Ttp]{number}
function    f_{identifier}
whitespace  [ \t\n]+


%%

\n {yylineno++;}
[ \t]+ {}
"/*"(([^\*]*(\*[^\/])?)*)"*/" {}
\/\/.* {}


[-+*/\(\)\[\]{}%,.:;!<>=&|] {return yytext[0];}

if	    {return IF;}
var         {return VAR;}
return      {return RETURN;}
goto	      {return GOTO;}
call        {return CALL;}
end	      {return END;}
param	      {return PARAM;}
{label}       {yylval.sValue=strdup(yytext); return LABEL; }
{number}      {yylval.iValue=atoi(yytext);return NUMBER;}
{variable}    {yylval.sValue=strdup(yytext); return IDENT;}
{function}    {yylval.sValue=strdup(yytext); return FUNC; }


<<EOF>> {return END;}


%%

int yywrap()
{
	return 1;
}

void yyerror(char *message)
{
    printf("%d %s\n",yylineno,message);
    exit(0);
}

