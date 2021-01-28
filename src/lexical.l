%{	
    #include "syntax.tab.h"
    #include "parseTree.h"
    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>
    #include <assert.h>
    #include <stdarg.h>
    #include <stdio.h>

   #include <string.h>
    #include <stddef.h>
    extern Leaf* root;
    int haserror=1;
    int synError=0;
    int errorcount=0;
    int yycolumn=1;
    #define YY_USER_ACTION \
    yylloc.first_line=yylloc.last_line=yylineno; \
    yylloc.first_column=yycolumn; \
    yylloc.last_column=yycolumn+yyleng-1; \
    yycolumn+=yyleng;
%}

%option yylineno
type ("float"|"int")
letter [A-Za-z]
digit [0-9]
space [ \r\t]
ws {space}+
int8 [0]([0-7])([0-7])*
int16 (([0][x])|([0][X]))([A-Fa-f]|[0-9])([A-Fa-f]|[0-9])*
int ([1-9]({digit})*)|[0]
float ({digit}*\.{digit}*[eE]([+]|[-])?{digit}{digit}*)|({digit}{digit}*\.{digit}{digit}*)
id ({letter}|[_])({letter}|{digit}|[_])*
semi ";"
comma ","
assignop "="
relop ">"|"<"|">="|"<="|"=="|"!="
plus "+"
minus "-"
star "*"
div "/"
and "&&"
or "||"
dot "."
not "!"
lp "("
rp ")"
lb "["
rb "]"
lc "{"
rc "}"
struct "struct"
return "return"
if "if"
else "else"
while "while"
commentbegin "/*"
commentelement .|\n
commentend "*/"
%x comment
%x commentinline
%%
{ws} {}
"/*" BEGIN(comment);
<comment>[^*\n\r]* 
<comment>"*"+[^*/\n\r]*   
<comment>[\n\r] yycolumn=1;
<comment>"*"+"/" BEGIN(INITIAL);
"//" BEGIN(commentinline);
<commentinline>[^\n\r]*
<commentinline>[\n\r] BEGIN(INITIAL); yycolumn=1;
[\n\r] {yycolumn=1;}
{semi} {yylval.node=newLeaf(yylineno,NO_VALUE,"SEMI",yytext);return SEMI;}
{comma} {yylval.node=newLeaf(yylineno,NO_VALUE,"COMMA",yytext);return COMMA;}
{assignop} {yylval.node=newLeaf(yylineno,NO_VALUE,"ASSIGNOP",yytext);return ASSIGNOP;}
{relop} {yylval.node=newLeaf(yylineno,NO_VALUE,"RELOP",yytext);return RELOP;}
{plus} {yylval.node=newLeaf(yylineno,NO_VALUE,"PLUS",yytext);return PLUS;}
{minus} {yylval.node=newLeaf(yylineno,NO_VALUE,"MINUS",yytext);return MINUS;}
{star} {yylval.node=newLeaf(yylineno,NO_VALUE,"STAR",yytext);return STAR;}
{div} {yylval.node=newLeaf(yylineno,NO_VALUE,"DIV",yytext);return DIV;}
{and} {yylval.node=newLeaf(yylineno,NO_VALUE,"AND",yytext);return AND;}
{or} {yylval.node=newLeaf(yylineno,NO_VALUE,"OR",yytext);return OR;}
{dot} {yylval.node=newLeaf(yylineno,NO_VALUE,"DOT",yytext);return DOT;}
{not} {yylval.node=newLeaf(yylineno,NO_VALUE,"NOT",yytext);return NOT;}
{lp} {yylval.node=newLeaf(yylineno,NO_VALUE,"LP",yytext);return LP;}
{rp} {yylval.node=newLeaf(yylineno,NO_VALUE,"RP",yytext);return RP;}
{lb} {yylval.node=newLeaf(yylineno,NO_VALUE,"LB",yytext);return LB;}
{rb} {yylval.node=newLeaf(yylineno,NO_VALUE,"RB",yytext);return RB;}
{lc} {yylval.node=newLeaf(yylineno,NO_VALUE,"LC",yytext);return LC;}
{rc} {yylval.node=newLeaf(yylineno,NO_VALUE,"RC",yytext);return RC;}
{struct} {yylval.node=newLeaf(yylineno,NO_VALUE,"STRUCT",yytext);return STRUCT;}
{return} {yylval.node=newLeaf(yylineno,NO_VALUE,"RETURN",yytext);return RETURN;}
{if} {yylval.node=newLeaf(yylineno,NO_VALUE,"IF",yytext);return IF;}
{else} {yylval.node=newLeaf(yylineno,NO_VALUE,"ELSE",yytext);return ELSE;}
{while} {yylval.node=newLeaf(yylineno,NO_VALUE,"WHILE",yytext);return WHILE;}
{type} {yylval.node=newLeaf(yylineno,TYPE_TYPE,"TYPE",yytext);return TYPE;}
{float} {yylval.node=newLeaf(yylineno,TYPE_FLOAT,"FLOAT",yytext);return FLOAT;}
{int8} {yylval.node=newLeaf(yylineno,TYPE_INT8,"INT",yytext);return INT;}
{int16} {yylval.node=newLeaf(yylineno,TYPE_INT16,"INT",yytext);return INT;}
{int} {yylval.node=newLeaf(yylineno,TYPE_INT,"INT",yytext);return INT;}
{id} {yylval.node=newLeaf(yylineno,TYPE_ID,"ID",yytext);return ID;}

%%
int main(int argc, char ** argv)
{	
  if (argc <= 1) {
        yyparse();
        return 1;
    }

    FILE* f = fopen(argv[1], "r");
    yyrestart(f);
    yyparse();
    outfile=fopen(argv[2],"w");
 //  printTree(root,0);
  
   
  	v_table=(V_Table*)malloc(sizeof(V_Table));

  	//s_table=(S_Table*)malloc(sizeof(S_Table));
  	f_table=(F_Table*)malloc(sizeof(F_Table));
    parseTree(root);
     fclose(outfile);
   // printV();
  //printS(1);
  // printFunction(functions->next);
    return 0;
}


