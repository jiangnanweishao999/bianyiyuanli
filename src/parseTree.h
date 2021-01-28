#include <stdlib.h>
typedef struct VARIABLE{
	char* name;
	int size;
}Variable;
typedef struct FUNCTION{
	char* name;
}Function;
int temporary=0;
int label=0;
typedef struct V_TABLE{
	Variable* v;
	struct V_TABLE* next;
}V_Table;
typedef struct F_TABLE{
	Function* f;
	struct F_TABLE* next;
}F_Table;
V_Table* v_table;
F_Table* f_table;
FILE* outfile;
char* addstr(char*a,char* b);
void freed(char* s);
Variable* newV(char* name,int size){
	Variable* v=(Variable*)malloc(sizeof(Variable));
	v->name=name;
	v->size=size;
	return v;
}
Function* newF(char* name){
	Function* f=(Function*)malloc(sizeof(Function));
	f->name=name;
	return f;
}
void output(char* output){
	fputs(output,outfile);
	freed(output);
}
void addV(Variable* v){
	V_Table* v_t=v_table;
	while(v_t->v!=NULL){
		v_t=v_t->next;
	}
	v_t->v=v;
	V_Table* v_Table=(V_Table*)malloc(sizeof(V_Table));
	v_t->next=v_Table;
}

Variable* getV(char* name){
	V_Table* v_t=v_table;
	while(v_t->v!=NULL){
		//fprintf(stderr,"%s\n",v_t->v->name);
		if(strcmp(v_t->v->name,name)==0){
			return v_t->v;
		}
		v_t=v_t->next;
	}
}
void printV(V_Table* v){
	if(v->next==NULL){
	return ;
	}
	//fprintf(stderr,"Variable: %s\n",v->v->name);
	printV(v->next);
}
Function* getF(char* name){
	F_Table* f_t=f_table;
	while(f_t->f!=NULL){
		if(strcmp(f_t->f->name,name)==0){
			return f_t->f;
		}
		f_t=f_t->next;
	}
}
char* addstr(char* a,char* b){
	char* res=(char*)malloc(sizeof(char)*(strlen(a)+strlen(b)+1));
	memset(res, '\0', sizeof(res));
	strcat(res,a);
	strcat(res,b);
	//fprintf(stderr,"res: %s\n",res);
	return res;
}
char* itoa(int i){
	char* res;
	res=(char*)malloc(sizeof(char)*10);
	memset(res, '\0', sizeof(res));
	sprintf(res,"%d",i);
	return res;
}
char* newT(){
	temporary++;
	return addstr("t",itoa(temporary));
}
char* newL(){
	label++;
	return addstr("label",itoa(label));
}
void freed(char* s){
	if(s!=""){
		free(s);
	}
}
void parseExtDefList(Leaf* ExtDefList);
void parseExtDef(Leaf* ExtDef);
void parseExtDecList(Leaf* ExtDecList);
Variable* parseVarDec(Leaf* VarDec);
void parseFunDec(Leaf* FunDec);
char* parseVarList(Leaf* VarList);
char* parseParamDec(Leaf* ParamDec);
char* parseCompSt(Leaf* CompSt);
char* parseDefList(Leaf* DefList);
char* parseDef(Leaf* Def);
char* parseDecList(Leaf* DecList);
char* parseDec(Leaf* Dec);
char* parseStmtList(Leaf* StmtList);
char* parseStmt(Leaf* Stmt);
char* parseCond(Leaf* Exp,char* label_true,char* label_false);
char* parseExp(Leaf* Exp,char** place,int i);
char* parseArgs(Leaf* Args,char* list[],int lo);
void parseTree(Leaf* Program){
	parseExtDefList(Program->child);
}
void parseExtDefList(Leaf* ExtDefList){
	if(ExtDefList->child!=NULL){
		Leaf* ExtDef=ExtDefList->child;
		parseExtDef(ExtDef);
		parseExtDefList(ExtDef->next);
	}
}
void parseExtDef(Leaf* ExtDef){
	Leaf* Specifier=ExtDef->child;
	if(strcmp(Specifier->next->name,"ExtDecList")==0){
		Leaf* ExtDecList=Specifier->next;
		parseExtDecList(ExtDecList);
	}
	else if(strcmp(Specifier->next->name,"FunDec")==0){
		Leaf* FunDec=Specifier->next;
		parseFunDec(FunDec);
		Leaf* CompSt=FunDec->next;
		char* res=parseCompSt(CompSt);
		output(res);
	}
	else{
	
	}
}
void parseExtDecList(Leaf* ExtDecList){
	Leaf* VarDec=ExtDecList->child;
	Variable* v=parseVarDec(VarDec);
	//addV(v);
	if(VarDec->next!=NULL){
		parseExtDecList(VarDec->next->next);
	}
}
Variable* parseVarDec(Leaf* VarDec){
	Leaf* ID=VarDec->child;

	if(ID->next==NULL){

		return newV(ID->value,0);
	}
	else{
		Leaf* SIZE=ID->next->next;
		return newV(ID->value,atoi(SIZE->value));
	}
}
void parseFunDec(Leaf* FunDec){
	Leaf* ID=FunDec->child;
	output(addstr(addstr("FUNCTION ",ID->value)," :\n"));
	if(strcmp(ID->next->next->name,"VarList")==0){
		Leaf* VarList=ID->next->next;
		output(parseVarList(VarList));
	}
}
char* parseVarList(Leaf* VarList){
	Leaf* ParamDec=VarList->child;
	char* code1=parseParamDec(ParamDec);
	if(ParamDec->next!=NULL){
		char* code2=parseVarList(ParamDec->next->next);
		char* res=addstr(code2,code1);
		freed(code1);
		freed(code2);
		return res;
	}
	return code1;
}
char* parseParamDec(Leaf* ParamDec){
	Leaf* Specifier=ParamDec->child;
	Leaf* VarDec=Specifier->next;
	Variable* v=parseVarDec(VarDec);
	//addV(v);
	return addstr(addstr("PARAM ",v->name),"\n");
}
char* parseCompSt(Leaf* CompSt){
	Leaf* DefList=CompSt->child->next;
	Leaf* StmtList=DefList->next;

	char* code1=parseDefList(DefList);
	char* code2=parseStmtList(StmtList);

	char* res=addstr(code1,code2);

	freed(code1);
	freed(code2);
	return res;
}
char* parseDefList(Leaf* DefList){
	if(DefList->child!=NULL){
		Leaf* Def=DefList->child;
		char* code1=parseDef(Def);
		char* code2=parseDefList(Def->next);
		char* res=addstr(code1,code2);
		freed(code1);
		freed(code2);
		return res;
	}
	return "";
}
char* parseDef(Leaf* Def){
	Leaf* Specifier=Def->child;
	Leaf* DecList=Specifier->next;
	return parseDecList(DecList);
}
char* parseDecList(Leaf* DecList){
	Leaf* Dec=DecList->child;
	char* code1=parseDec(Dec);
	if(Dec->next!=NULL){
		char* code2=parseDecList(Dec->next->next);
		char* res=addstr(code1,code2);
		freed(code1);
		freed(code2);
		return res;
	}
	return code1;
}
char* parseDec(Leaf* Dec){
	Leaf* VarDec=Dec->child;
	Variable* v=parseVarDec(VarDec);
	//addV(v);
	char* v_name=addstr(v->name,"");
	char* code1="";
	if(v->size!=0){
		code1=addstr(addstr(addstr("DEC ",v->name),addstr(" ",itoa(4*(v->size)))),"\n");
	}
	if(VarDec->next!=NULL){
		char* code2=parseExp(VarDec->next->next,&v_name,1);
		char* res=addstr(code1,code2);
		freed(code1);
		freed(code2);
		return res;
	}
	//assignlop new temporary
	return code1;
}
char* parseStmtList(Leaf* StmtList){

	if(StmtList->child!=NULL){
		Leaf* Stmt=StmtList->child;
		char* code1=parseStmt(Stmt);
		char* code2=parseStmtList(Stmt->next);

		char* res=addstr(code1,code2);
		freed(code1);
		freed(code2);
		
		return res;
	}
	return "";
}
char* parseStmt(Leaf* Stmt){
	//     CompSt
	if(strcmp(Stmt->child->name,"CompSt")==0){
	Leaf* CompSt=Stmt->child;
	return parseCompSt(CompSt);
	}
	//	Exp SEMI
	else if(strcmp(Stmt->child->name,"Exp")==0){

	Leaf* Exp=Stmt->child;
	char* a="";
	return parseExp(Exp,&a,1);
	}
	//	RETURN Exp SEMI
	else if(strcmp(Stmt->child->name,"RETURN")==0){
	Leaf* Exp=Stmt->child->next;
	char* t1=newT();
	char* code1=parseExp(Exp,&t1,1);
	char* code2=addstr(addstr("RETURN ",t1),"\n");
	char* res=addstr(code1,code2);
	freed(t1);
	freed(code1);
	freed(code2);
	return res;
	}
	//    WHILE LP Exp RP Stmt  
	else if(strcmp(Stmt->child->name,"WHILE")==0){
	Leaf* Exp=Stmt->child->next->next;
	Leaf* Stmt1=Exp->next->next;
	char* l1=newL();
	char* l2=newL();
	char* l3=newL();
	char* code1=parseCond(Exp,l2,l3);
	char* code2=parseStmt(Stmt1);
	char* res=addstr(addstr(addstr(addstr(addstr("LABEL ",l1)," :\n"),code1),addstr(addstr(addstr("LABEL ",l2)," :\n"),code2)),addstr(addstr(addstr("GOTO ",l1),"\n"),addstr(addstr("LABEL ",l3)," :\n")));
	freed(l1);
	freed(l2);
	freed(l3);
	freed(code1);
	freed(code2);
	return res;
	}        
	//	IF LP Exp RP Stmt    
        else if(Stmt->child->next->next->next->next->next==NULL){
	Leaf* Exp=Stmt->child->next->next;
	Leaf* Stmt1=Exp->next->next; 
        char* l1=newL();
        char* l2=newL();
        char* code1=parseCond(Exp,l1,l2);
        char* code2=parseStmt(Stmt1);
        char* res=addstr(addstr(code1,addstr(addstr("LABEL ",l1)," :\n")),addstr(code2,addstr(addstr("LABEL ",l2)," :\n")));
        freed(l1);
        freed(l2);
        freed(code1);
        freed(code2);
        return res;
        }
	//    IF LP Exp RP Stmt ELSE Stmt  
    	else{
    	Leaf* Exp=Stmt->child->next->next;
	Leaf* Stmt1=Exp->next->next; 
	Leaf* Stmt2=Stmt1->next->next;
	char* l1=newL();
	char* l2=newL();
	char* l3=newL();
	char* code1=parseCond(Exp,l1,l2);
	char* code2=parseStmt(Stmt1);
	char* code3=parseStmt(Stmt2);
	char* part1=addstr(addstr(addstr(code1,addstr(addstr("LABEL ",l1)," :\n")),addstr(code2,addstr("GOTO ",l3))),"\n");
	char* part2=addstr(addstr(addstr("LABEL ",l2)," :\n"),addstr(code3,addstr(addstr("LABEL ",l3)," :\n")));
	char* res=addstr(part1,part2);
	freed(l1);
	freed(l2);
	freed(l3);
	freed(code1);
	freed(code2);
	freed(code3);
	freed(part1);
	freed(part2);
	return res;
    	}
}
char* parseCond(Leaf* Exp,char* label_true,char* label_false){
     //               Exp RELOP Exp
     if((Exp->child->next!=NULL)&&strcmp(Exp->child->next->name,"RELOP")==0){
     	char* t1=newT();
    	char* t2=newT();
    	Leaf* Exp1=Exp->child;
    	Leaf* Exp2=Exp1->next->next;
   	char* code1=parseExp(Exp1,&t1,1);
   	char* code2=parseExp(Exp2,&t2,1);
   	char* op=Exp->child->next->value;
   	char* code3=addstr(addstr(addstr("IF ",t1),addstr(" ",op)),addstr(addstr(" ",t2),addstr(" GOTO ",label_true)));
   	char* res=addstr(addstr(addstr(code1,code2),addstr(code3,"\n")),addstr(addstr("GOTO ",label_false),"\n"));
     	freed(t1);
     	freed(t2);
	freed(code1);
      	freed(code2);
     	freed(code3);
     	return res;
     }
     //               NOT Exp
     else if(strcmp(Exp->child->name,"NOT")==0){
     	return parseCond(Exp->child->next,label_false,label_true);
     }  
     //               Exp AND Exp 
     else if((Exp->child->next!=NULL)&&strcmp(Exp->child->next->name,"AND")==0){
     	char* l1=newL();
     	Leaf* Exp1=Exp->child;
    	Leaf* Exp2=Exp1->next->next;
     	char* code1=parseCond(Exp1,l1,label_false);
     	char* code2=parseCond(Exp2,label_true,label_false);
     	char* res= addstr(addstr(code1,addstr(addstr("LABEL ",l1)," :\n")),code2);
     	freed(l1);
     	freed(code1);
     	freed(code2);
     	return res;
     }
     //               Exp OR Exp  
     else if((Exp->child->next!=NULL)&&strcmp(Exp->child->next->name,"OR")==0){
     	char* l1=newL();
     	Leaf* Exp1=Exp->child;
    	Leaf* Exp2=Exp1->next->next;
    	char* code1=parseCond(Exp1,label_true,l1);
     	char* code2=parseCond(Exp2,label_true,label_false);
     	char* res=addstr(addstr(code1,addstr(addstr("LABEL ",l1)," :\n")),code2);
	freed(l1);
	freed(code1);
	freed(code2);
     	return res;
     }
     else{
     	char* t1=newT();
     	char* code1=parseExp(Exp,&t1,1);
     	char* code2=addstr(addstr(addstr("IF ",t1),addstr(" != #0 GOTO ",label_true)),"\n");
     	char* res=addstr(addstr(code1,code2),addstr(addstr("GOTO ",label_false),"\n"));
	freed(t1);
	freed(code1);
	freed(code2);
     	return res;
     }
}
char* parseExp(Leaf* Exp,char** place,int i){  
    if(strcmp(*place,"")==0){
    	*place=newT();
    }
    //               INT                  
    if(strcmp(Exp->child->name,"INT")==0){
	Leaf* INT=Exp->child;
	return addstr(addstr(addstr(*place," := "),addstr("#",INT->value)),"\n");
    }
    //               ID   
    //               ID LB Exp RB  
    else if(strcmp(Exp->child->name,"ID")==0&&(Exp->child->next==NULL||strcmp(Exp->child->next->name,"LB")==0)){

    	char* v_name=Exp->child->value;
    	if(Exp->child->next==NULL){
    	if(i==1){
    	return addstr(addstr(addstr(*place," := "),v_name),"\n");
    	}
    	else{
    	*place=addstr(v_name,"");
    	return "";
    	}
    	}
    	else{
    	char* t1=newT();
    	char* t2=newT();
    	char* t3=newT();
    	Leaf* Exp1=Exp->child->next->next;
    	char* code1=parseExp(Exp1,&t1,1);
    	char* code2=addstr(addstr(t2," := "),addstr(t1," * #4\n"));
    	char* code3=addstr(addstr(addstr(addstr(t3," := &"),v_name),addstr(" + ",t2)),"\n");
    	//t3 is address
    	if(i==1){
    		char* code4=addstr(addstr(addstr(*place," := "),addstr("*",t3)),"\n");
    		char* res=addstr(addstr(code1,code2),addstr(code3,code4));
    		freed(t1);
    		freed(t2);
    		freed(t3);
    		freed(code1);
    		freed(code2);
    		freed(code3);
    		freed(code4);
    		return res;
    	}
    	else{
    		*place=addstr("*",t3);
    		char* res= addstr(addstr(code1,code2),code3);
    		freed(t1);
    		freed(t2);
    		freed(t3);
    		freed(code1);
    		freed(code2);
    		freed(code3);
    		return res;
	}
    	}
    }
    //               Exp ASSIGNOP Exp 
    else if(strcmp(Exp->child->next->name,"ASSIGNOP")==0){
    	Leaf* Exp1=Exp->child;
    	Leaf* Exp2=Exp1->next->next;
    	char* t1=newT();
    	char* t2=newT();
    	char* code1=parseExp(Exp1,&t1,0);
    	char* code2=parseExp(Exp2,&t2,1);
    	char* code3=addstr(addstr(addstr(addstr(t1," := "),t2),"\n"),addstr(addstr(addstr(*place," := "),t1),"\n"));
    	char* res=addstr(addstr(code1,code2),code3);
    	freed(t1);
    	freed(t2);
    	freed(code1);
    	freed(code2);
    	freed(code3);
    	return res;
    }
    //               Exp PLUS Exp   
    else if(strcmp(Exp->child->next->name,"PLUS")==0){
    Leaf* Exp1=Exp->child;
    	Leaf* Exp2=Exp1->next->next;
    	char* t1=newT();
    	char* t2=newT();
    	char* code1=parseExp(Exp1,&t1,1);
    	char* code2=parseExp(Exp2,&t2,1);
    	char* code3=addstr(addstr(addstr(*place," := "),addstr(addstr(t1," + "),t2)),"\n");
    	char* res=addstr(addstr(code1,code2),code3);
    	freed(t1);
    	freed(t2);
    	freed(code1);
    	freed(code2);
    	freed(code3);
    	return res;
    }                     
    //               Exp MINUS Exp     
    else if(strcmp(Exp->child->next->name,"MINUS")==0){
    Leaf* Exp1=Exp->child;
    	Leaf* Exp2=Exp1->next->next;
    	char* t1=newT();
    	char* t2=newT();
    	char* code1=parseExp(Exp1,&t1,1);
    	char* code2=parseExp(Exp2,&t2,1);
    	char* code3=addstr(addstr(addstr(*place," := "),addstr(addstr(t1," - "),t2)),"\n");
    	char* res= addstr(addstr(code1,code2),code3);
	freed(t1);
	freed(t2);
	freed(code1);
	freed(code2);
	freed(code3);
    	return res;
    }                 
    //               Exp STAR Exp 
    else if(strcmp(Exp->child->next->name,"STAR")==0){
    Leaf* Exp1=Exp->child;
    	Leaf* Exp2=Exp1->next->next;
    	char* t1=newT();
    	char* t2=newT();
    	char* code1=parseExp(Exp1,&t1,1);
    	char* code2=parseExp(Exp2,&t2,1);
    	char* code3=addstr(addstr(addstr(*place," := "),addstr(addstr(t1," * "),t2)),"\n");
    	char* res=addstr(addstr(code1,code2),code3);
 	freed(t1);
 	freed(t2);
 	freed(code1);
 	freed(code2);
 	freed(code3);
    	return res;
    }                        
    //               Exp DIV Exp  
    else if(strcmp(Exp->child->next->name,"DIV")==0){
    	Leaf* Exp1=Exp->child;
    	Leaf* Exp2=Exp1->next->next;
    	char* t1=newT();
    	char* t2=newT();
    	char* code1=parseExp(Exp1,&t1,1);
    	char* code2=parseExp(Exp2,&t2,1);
    	char* code3=addstr(addstr(addstr(*place," := "),addstr(addstr(t1," / "),t2)),"\n");
    	char* res=addstr(addstr(code1,code2),code3);
	freed(t1);
 	freed(t2);
 	freed(code1);
 	freed(code2);
 	freed(code3);
    	return res;
    }
    //               MINUS Exp
    else if(strcmp(Exp->child->name,"MINUS")==0){
    	char* t1=newT();
    	Leaf* Exp1=Exp->child->next;
    	char* code1=parseExp(Exp1,&t1,1);
    	char* code2=addstr(addstr(addstr(*place," := #0 - "),t1),"\n");
    	char* res=addstr(code1,code2);
	freed(t1);
 	freed(code1);
 	freed(code2);
    	return res;
    }
    //		     Exp RELOP Exp	
    //		     Exp OR Exp
    //		     Exp AND Exp
    else if(strcmp(Exp->child->next->name,"RELOP")==0||strcmp(Exp->child->next->name,"AND")==0||strcmp(Exp->child->next->name,"OR")==0){
    	char* l1=newL();
    	char* l2=newL();
    	char* code0=addstr(*place," := #0\n");
    	char* code1=parseCond(Exp,l1,l2);
    	char* code2=addstr(addstr(addstr("LABEL ",l1)," :\n"),addstr(*place," := #1\n"));
    	char* res=addstr(addstr(code0,code1),addstr(code2,addstr(addstr("LABEL ",l2)," :\n")));
	freed(l1);
 	freed(l2);
 	freed(code0);
 	freed(code1);
 	freed(code2);
    	return res;
    }
    //		    NOT Exp
    else if(strcmp(Exp->child->name,"NOT")==0){
    	char* l1=newL();
    	char* l2=newL();
    	char* code0=addstr(*place," := #0\n");
    	char* code1=parseCond(Exp,l1,l2);
    	char* code2=addstr(addstr(addstr("LABEL ",l1)," :\n"),addstr(*place," := #1\n"));
    	char* res= addstr(addstr(code0,code1),addstr(code2,addstr(addstr("LABEL ",l2)," :\n")));
  	freed(l1);
 	freed(l2);
 	freed(code0);
 	freed(code1);
 	freed(code2);
    	return res;
    }
    //               LP Exp RP
    else if(strcmp(Exp->child->name,"LP")==0){
    	return parseExp(Exp->child->next,place,i);
    }                    
    //               ID LP Args RP
    else if(Exp->child->next->next!=NULL&&strcmp(Exp->child->next->next->name,"Args")==0){
    	Leaf* Args=Exp->child->next->next;

    	char* arg_list[100];

    	for(int i=0;i<100;i++){arg_list[i]="";}
    	char* code1=parseArgs(Args,arg_list,0);
    	Leaf* ID=Exp->child;
    	if(strcmp(ID->value,"write")==0){
    		char* res=addstr(code1,addstr(addstr("WRITE ",arg_list[0]),"\n"));
    		return res;
    	}
    	char* code2="";
    	for(int i=0;i<100;i++){

    		if(arg_list[i]!=""){
    			code2=addstr(code2,addstr(addstr("ARG ",arg_list[i]),"\n"));
    		}
    	}
    	char* res=addstr(addstr(code1,code2),addstr(addstr(*place,addstr(" := CALL ",ID->value)),"\n"));
	freed(code1);
	freed(code2);
	freed(*arg_list);
    	return res;
    }         
    //               ID LP RP                  
    else if(Exp->child->next->next!=NULL&&strcmp(Exp->child->next->next->name,"RP")==0){
    	Leaf* ID=Exp->child;
    	if(strcmp(ID->value,"read")==0){
    		return addstr(addstr("READ ",*place),"\n");
    	}
    	return addstr(addstr(*place,addstr(" := CALL ",ID->value)),"\n");
    }  
    else{
    return "";
    }
}
char* parseArgs(Leaf* Args,char* list[],int lo){
	Leaf* Exp=Args->child;
	
	//getArgs
	char* t1=newT();
	char* code1=parseExp(Exp,&t1,1);
	list[lo]=t1;
	if(Exp->next!=NULL){
		char* code2=parseArgs(Exp->next->next,list,lo+1);
		char* res=addstr(code1,code2);

		return res;
	}
	return code1;
}
