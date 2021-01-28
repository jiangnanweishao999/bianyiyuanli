%locations

%{
	typedef enum Type{
	TYPE_INT,
	TYPE_INT8,
	TYPE_INT16,
	TYPE_FLOAT,
	TYPE_TYPE,
	TYPE_ID,
	NO_VALUE,
	NOT_TOKEN
}Type;
typedef struct leaf{
	int lineNo;
	Type type;
	char* name;
	char* value;
	struct leaf* child;
	struct leaf* next;
}Leaf;

    void yyerror();
    int yyparse(void);
    void printTree(Leaf* branch,int height);
    Leaf* newBranch(int lineNo, Type type, char* name,int number,...);
    Leaf* newLeaf(int lineNo,Type type,char* name,char* value);
    #include <stdio.h>
    #include "lex.yy.c"

    Leaf* root;
%}
%union{
    Leaf* node;
}
%token <node> INT
%token <node> FLOAT
%token <node> ID
%token <node> TYPE
%token <node> COMMA
%token <node> DOT
%token <node> SEMI
%token <node> RELOP
%token <node> ASSIGNOP
%token <node> PLUS MINUS STAR DIV
%token <node> AND OR NOT 
%token <node> LP RP LB RB LC RC
%token <node> IF
%token <node> ELSE
%token <node> WHILE
%token <node> STRUCT
%token <node> RETURN

%type <node> Program ExtDefList ExtDef ExtDecList   
%type <node> Specifier StructSpecifier Tag OptTag
%type <node> VarDec FunDec VarList ParamDec        
%type <node> CompSt StmtList Stmt                   
%type <node> DefList Def Dec DecList                
%type <node> Exp Args                              

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT MINUS_TWO
%left DOT
%left LB RB
%left LP RP
%nonassoc ELSE
%%

Program:            ExtDefList                              { $$ = newBranch(@$.first_line, NOT_TOKEN, "Program", 1, $1); root = $$; }
    ; 
ExtDefList:         ExtDef ExtDefList                       { $$ = newBranch(@$.first_line, NOT_TOKEN, "ExtDefList", 2, $1, $2); }
  //  |                                                       { $$ = NULL; } 
    |                                                       { $$ = newBranch(@$.first_line, NOT_TOKEN, "ExtDefList", 0); }
    ; 
ExtDef:             Specifier ExtDecList SEMI               { $$ = newBranch(@$.first_line, NOT_TOKEN, "ExtDef", 3, $1, $2, $3); }
    |               Specifier SEMI                          { $$ = newBranch(@$.first_line, NOT_TOKEN, "ExtDef", 2, $1, $2); }
    |               Specifier FunDec CompSt                 { $$ = newBranch(@$.first_line, NOT_TOKEN, "ExtDef", 3, $1, $2, $3); }
    ; 
ExtDecList:         VarDec                                  { $$ = newBranch(@$.first_line, NOT_TOKEN, "ExtDecList", 1, $1); }
    |               VarDec COMMA ExtDecList                 { $$ = newBranch(@$.first_line, NOT_TOKEN, "ExtDecList", 3, $1, $2, $3); }
  //  |               VarDec error ExtDecList                 {yyerror();}
    ; 

// Specifiers
Specifier:          TYPE                                    { $$ = newBranch(@$.first_line, NOT_TOKEN, "Specifier", 1, $1); }
    |               StructSpecifier                         { $$ = newBranch(@$.first_line, NOT_TOKEN, "Specifier", 1, $1); }
    ; 
StructSpecifier:    STRUCT OptTag LC DefList RC             { $$ = newBranch(@$.first_line, NOT_TOKEN, "StructSpecifier", 5, $1, $2, $3, $4, $5); }
    |               STRUCT Tag                              { $$ = newBranch(@$.first_line, NOT_TOKEN, "StructSpecifier", 2, $1, $2); }
   // |               STRUCT OptTag LC error RC               {yyerror();}
    ; 
OptTag:             ID                                      { $$ = newBranch(@$.first_line, NOT_TOKEN, "OptTag", 1, $1); }
    |                                                       { $$ = newBranch(@$.first_line, NOT_TOKEN, "OptTag", 0); }
  //  |                                                       { $$ = NULL; }
    ; 
Tag:                ID                                      { $$ = newBranch(@$.first_line, NOT_TOKEN, "Tag", 1, $1); }
    ; 

// Declarators
VarDec:             ID                                      { $$ = newBranch(@$.first_line, NOT_TOKEN, "VarDec", 1, $1); }
    //|               VarDec LB INT RB                        { $$ = newBranch(@$.first_line, NOT_TOKEN, "VarDec", 4, $1, $2, $3, $4); }
    |               ID LB INT RB                            {$$ =newBranch(@$.first_line,NOT_TOKEN,"VarDec",4, $1, $2, $3, $4); }
    ; 
FunDec:             ID LP VarList RP                        { $$ = newBranch(@$.first_line, NOT_TOKEN, "FunDec", 4, $1, $2, $3, $4); }
    |               ID LP RP                                { $$ = newBranch(@$.first_line, NOT_TOKEN, "FunDec", 3, $1, $2, $3); }
   // |               ID LP error RP                          {yyerror();}
    ; 
VarList:            ParamDec COMMA VarList                  { $$ = newBranch(@$.first_line, NOT_TOKEN, "VarList", 3, $1, $2, $3); }
    |               ParamDec                                { $$ = newBranch(@$.first_line, NOT_TOKEN, "VarList", 1, $1); }
    ; 
ParamDec:           Specifier VarDec                        { $$ = newBranch(@$.first_line, NOT_TOKEN, "ParamDec", 2, $1, $2); }
    ; 
// Statements
CompSt:             LC DefList StmtList RC                  { $$ = newBranch(@$.first_line, NOT_TOKEN, "CompSt", 4, $1, $2, $3, $4); }
    ; 
StmtList:           Stmt StmtList                           { $$ = newBranch(@$.first_line, NOT_TOKEN, "StmtList", 2, $1, $2); }
    |                                                       { $$ = newBranch(@$.first_line, NOT_TOKEN, "StmtList", 0); }
  //  |                                                       { $$ = NULL; }
    ; 
Stmt:               Exp SEMI                                { $$ = newBranch(@$.first_line, NOT_TOKEN, "Stmt", 2, $1, $2); }
    |               CompSt                                  { $$ = newBranch(@$.first_line, NOT_TOKEN, "Stmt", 1, $1); }
    |               RETURN Exp SEMI                         { $$ = newBranch(@$.first_line, NOT_TOKEN, "Stmt", 3, $1, $2, $3); }    
    |               IF LP Exp RP Stmt                       { $$ = newBranch(@$.first_line, NOT_TOKEN, "Stmt", 5, $1, $2, $3, $4, $5); }
    |               IF LP Exp RP Stmt ELSE Stmt             { $$ = newBranch(@$.first_line, NOT_TOKEN, "Stmt", 7, $1, $2, $3, $4, $5, $6, $7); }
    |               WHILE LP Exp RP Stmt                    { $$ = newBranch(@$.first_line, NOT_TOKEN, "Stmt", 5, $1, $2, $3, $4, $5); }
   // |               IF LP error RP Stmt ELSE Stmt           {yyerror();}
  //  |               WHILE LP error RP Stmt                  {yyerror();}
  //  |               RETURN error SEMI                       {yyerror();}
    ;
//Local Definitions
DefList:            Def DefList                             { $$ = newBranch(@$.first_line, NOT_TOKEN, "DefList", 2, $1, $2); }
    |                                                       { $$ = newBranch(@$.first_line, NOT_TOKEN, "DefList", 0); }
    //|                                                       { $$ = NULL; }
    ;     
Def:                Specifier DecList SEMI                  { $$ = newBranch(@$.first_line, NOT_TOKEN, "Def", 3, $1, $2, $3); }
   // |               error SEMI                              {yyerror();}
    ;
DecList:            Dec                                     { $$ = newBranch(@$.first_line, NOT_TOKEN, "DecList", 1, $1); }
    |               Dec COMMA DecList                       { $$ = newBranch(@$.first_line, NOT_TOKEN, "DecList", 3, $1, $2, $3); }
    ; 
Dec:                VarDec                                  { $$ = newBranch(@$.first_line, NOT_TOKEN, "Dec", 1, $1); }
    |               VarDec ASSIGNOP Exp                     { $$ = newBranch(@$.first_line, NOT_TOKEN, "Dec", 3, $1, $2, $3); }
    ; 
//Expressions
Exp:                Exp ASSIGNOP Exp                        { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 3, $1, $2, $3); }
    |               Exp AND Exp                             { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 3, $1, $2, $3); }
    |               Exp OR Exp                              { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 3, $1, $2, $3); }
    |               Exp RELOP Exp                           { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 3, $1, $2, $3); }
    |               Exp PLUS Exp                            { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 3, $1, $2, $3); }
    |               Exp MINUS Exp                           { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 3, $1, $2, $3); }
    |               Exp STAR Exp                            { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 3, $1, $2, $3); }
    |               Exp DIV Exp                             { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 3, $1, $2, $3); }
    |               LP Exp RP                               { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 3, $1, $2, $3); }//         
    |               MINUS Exp  %prec MINUS_TWO              { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 2, $1, $2); }//
    |               NOT Exp                                 { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 2, $1, $2); } //
    |               ID LP Args RP                           { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 4, $1, $2, $3, $4); }//
    |               ID LP RP                                { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 3, $1, $2, $3); }//
   // |               Exp LB Exp RB                           { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 4, $1, $2, $3, $4); }//
    |               ID LB Exp RB                           { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 4, $1, $2, $3, $4); }//
   // |               Exp DOT ID                              { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 3, $1, $2, $3); }//
    |               ID                                      { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 1, $1); }//
    |               INT                                     { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 1, $1); }//
   // |               FLOAT                                   { $$ = newBranch(@$.first_line, NOT_TOKEN, "Exp", 1, $1); }//
   // |               Exp LB error RB                         {yyerror();}
   // |               ID LP error RP                          {yyerror();}
   // |               LP error RP                             {yyerror();}
    ; 
Args :              Exp COMMA Args                          { $$ = newBranch(@$.first_line, NOT_TOKEN, "Args", 3, $1, $2, $3); }
    |               Exp                                     { $$ = newBranch(@$.first_line, NOT_TOKEN, "Args", 1, $1); }
    ; 
%%

Leaf* newBranch(int lineNo, Type type, char* name,int number,...){
	Leaf* branch=NULL;
	branch=(Leaf*)malloc(sizeof(Leaf));
	branch->name=(char*)malloc(sizeof(char)*(strlen(name)+1));
	branch->lineNo=lineNo;
	branch->type=type;
	branch->next=NULL;
	branch->child=NULL;
	strncpy(branch->name,name,strlen(name));
	if(number>0){
	va_list vaList;
	va_start(vaList,number);
	Leaf* thinBranch=va_arg(vaList,Leaf*);
	branch->child=thinBranch;
	for(int i=1;i<number;i++){
	    thinBranch->next=va_arg(vaList,Leaf*);
	    if(thinBranch->next !=NULL){
	        thinBranch=thinBranch->next;
	    }
	}
	}
	//printTree(branch,0);
	return branch;
}
Leaf* newLeaf(int lineNo,Type type,char* name,char* value){
	Leaf* leaf=NULL;
	leaf=(Leaf*)malloc(sizeof(Leaf));
	leaf->name=(char*)malloc(sizeof(char)*(strlen(name)));
	leaf->value=(char*)malloc(sizeof(char)*(strlen(value)));
	leaf->lineNo=lineNo;
	leaf->type=type;
	strncpy(leaf->name,name,strlen(name));
	strncpy(leaf->value,value,strlen(value));
	leaf->child=NULL;
	leaf->next=NULL;
	return leaf;
	}
void printTree(Leaf* branch,int height){
	if(branch==NULL){
		return;
	}
	for(int i=0;i<height;i++){
		fprintf(stderr,"  ");
	}
	if(branch->type==NOT_TOKEN){
		fprintf(stderr,"%s",branch->name);
		fprintf(stderr," (%d)\n",branch->lineNo);
	}
	else if(branch->type==TYPE_INT||branch->type==TYPE_ID||branch->type==TYPE_TYPE){
		fprintf(stderr,"%s",branch->name);
		fprintf(stderr,": %s\n",branch->value);
		}
	else if(branch->type==TYPE_FLOAT){
		fprintf(stderr,"%s",branch->name);
		fprintf(stderr,": %f\n",atof(branch->value));
		}
	else if(branch->type==TYPE_INT8){
		fprintf(stderr,"%s",branch->name);
		int num=0;
		sscanf(branch->value,"%o",&num);
		fprintf(stderr,": %d\n",num);
		}
	else if(branch->type==TYPE_INT16){
		fprintf(stderr,"%s",branch->name);
		int num=0;
		sscanf(branch->value,"%x",&num);
		fprintf(stderr,": %d\n",num);
		}
	else{
		fprintf(stderr,"%s\n",branch->name);
	}
	printTree(branch->child,height+1);
	printTree(branch->next,height);
}
void yyerror(){
   synError=1;
   fprintf(stderr,"Error type B at Line %d: Syntax error,%s,%d.\n",yylineno,yytext,yycolumn);
}
