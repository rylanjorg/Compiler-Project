%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdbool.h>
	#include <errno.h>
	#include <time.h>


	#include "Helpers/ConsoleFormating.h"
	#include "Helpers/Dictionary.h"

	#include "InstructionStack/ConstantOptimizations.h"
	#include "InstructionStack/ScopeManagement.h"
	
	#include "IRCode/IRcodeMain.h"
	#include "AST/AST.h"

	#include "SymbolTableManagement/GlobalSymbolTable.h"
	#include "SymbolTableManagement/ConditionalBlockTableManagement.h"
	#include "SymbolTableManagement/SymbolTableDebug.h"

	#include "MIPS/MIPSconvert.h"
	
	

	extern int yylex();
	extern int yyparse();
	extern FILE* yyin;
	FILE * IRcode;


	void yyerror(const char* s);
	char productionRule[100] = "";
%}

%union 
{
	int number;
	float float_number;
	char character;
	int boolean; // 0 for false, 1 for true

	char* string;
	struct ASTNode* ast;
}

%token <string> TYPE
%token <string> VOID
%token <string> RETURN
%token <string> IDENTIFIER
%token <char> SEMICOLON

%token <char> LCBRA
%token <char> RCBRA
%token <char> LPAR
%token <char> RPAR
%token <char> LBRACKET
%token <char> RBRACKET
%token <char> COMMA

%token <char> EQ
%token <string> OPERATOR
%token <string> CONDITIONAL_OPERATOR



// Token Types
%token <number> INT_LITERAL
%token <float_number> FLOAT_LITERAL
%token <character> CHAR_LITERAL
%token <boolean> BOOLEAN_LITERAL

%token WRITE
%token WHILE
%token IF
%token ELSE
%token UNROLL

%printer { fprintf(yyoutput, "%s", $$); } CONDITIONAL_OPERATOR;
%printer { fprintf(yyoutput, "%s", $$); } OPERATOR;
%printer { fprintf(yyoutput, "%s", $$); } IDENTIFIER;


%printer { fprintf(yyoutput, "%d", $$); } INT_LITERAL;
%printer { fprintf(yyoutput, "%f", $$); } FLOAT_LITERAL;
%printer { fprintf(yyoutput, "%c", $$); } CHAR_LITERAL;
%printer { fprintf(yyoutput, "%d", ($$) ? 1 : 0); } BOOLEAN_LITERAL;



//%type <string> funcScope

%type <ast> Program Block BlockNoReturn ConditionalBlock ConditionalExpression WhileBlock IfBlock ElseIfList ElseIfBlock BlockList ReturnStmt GlobalBlockList GlobVarDecl GlobalTypeExpr VarDecl ParamDeclList ParamDecl ParamList Param FuncDecl Stmt  StmtList TypeExpr Expr NonAssignExpr GlobNonAssignExpr

/*
	Production Rules:
	Program: FuncDeclList | Program FuncDeclList
	FuncDeclList: FuncDecl | FuncDecl FuncDeclList
	FuncDecl: TYPE ID LPAR ParamDeclList RPAR Block
	ParamDeclList: ParamDecl | ParamDecl COMMA ParamDeclList

	Block: LCBRA BlockList RCBRA
	BlockList: e | BlockList StmtList | BlockList VarDecl

	ParamDecl: Type ID | Type ID LBRA RBRA
	DeclList: Decl DeclList | Decl | e

	//dk
	Decl: VarDecl | StmtList
	VarDecl: TYPE IDENTIFIER SEMICOLON
	StmtList: stmt StmtList | stmt
	Stmt: SEMICOLON | Expr SEMICOLON
	Expr: IDENTIFER | IDENTIFIER EQ Expr | IDENTIFIER OPERATOR Expr | NUMBER | WRITE IDENTIFIER
	//
*/

%start Program

%%


//Program: GlobVarDeclList FuncDeclList 
Program: GlobalBlockList 
{ 
    //$$ = createNode($1, $2, NODE_PROGRAM);
	$$ = createNode(NULL, $1, NODE_PROGRAM);


    printf(ANSI_COLOR_BRIGHT_YELLOW"\n--- Current Symbol Table ---\n\n"ANSI_COLOR_RESET);
	ShowAllSymTabs(head);
    //PerformLivenessAnalysis(head);
	CleanUpAst(head, $1);
    ShowAllSymTabs(head);
    printf(ANSI_COLOR_BRIGHT_YELLOW"\n--- Abstract Syntax Tree ---\n\n"ANSI_COLOR_RESET);
    printAST($$, 0, 0);
    printf(ANSI_COLOR_BRIGHT_YELLOW"\n--- IR Code ---\n\n"ANSI_COLOR_RESET);
    WriteIRCodeToFile($$, head);
	HandleIRCodeForParameters(head);
	ConvertIRCodeToMIPS();
    //freeAST($$); //free non-vars
};

Block: {/*insert*/};

BlockNoReturn: {/*insert*/};

BlockList: {/*insert*/};

ConditionalBlock: {/*insert*/};

ConditionalExpression: {/*insert*/};

IfBlock: {/*insert*/};

ElseIfList: {/*insert*/};

ElseIfBlock: {/*insert*/};

WhileBlock: {/*insert*/};

ReturnStmt: {/*insert*/};

FuncDecl: {/*insert*/};

ParamDeclList: {/*insert*/};

ParamDecl: {/*insert*/};

ParamList: {/*insert*/};

Param: {/*insert*/};

GlobalBlockList: {/*insert*/};

GlobVarDecl: {/*insert*/};

GlobalTypeExpr: {/*insert*/};

VarDecl: {/*insert*/};

Stmt: {/*insert*/};

StmtList: {/*insert*/};

TypeExpr: {/*insert*/};

Expr: {/*insert*/};

NonAssignExpr: {/*insert*/};

GlobNonAssignExpr: {/*insert*/};

%%


void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
	exit(1);
}


int main(int argc, char**argv)
{
	clock_t start, end;
    double cpu_time_used;

    start = clock();

	#ifdef YYDEBUG
		yydebug = 1;
	#endif

	// Initialize the dictionary used for IRCode
	//initDictionary(&myDict);

	strcpy(globScope, "Global\0");
	
	// Initialize the global symbol table
    insertSymbolTable(&head, globScope, GlobalTable, 0);


	printf(ANSI_COLOR_BRIGHT_YELLOW "\n\n#####- COMPILER STARTED -#####\n\n" ANSI_COLOR_RESET);

	FILE *fileIR;

	// "w" means open for writing (creates if not exist)
	fileIR = fopen("OutputFiles/IRCode.ir", "w"); 

	if (fileIR == NULL ) {
		printf("Error opening IRCode.ir file: %s\n", strerror(errno));
		return 1;
	}


	fprintf(fileIR, "\n%s\n", "--- IR Code Start ---");
	fclose(fileIR);

	
	
	if (argc > 1){
	  if(!(yyin = fopen(argv[1], "r")))
          {
		perror(argv[1]);
		return(1);
	  }
	}

	yyparse();
	PrintSymbolTablesToFile();
	//freeSymbolTableVariables(); // free variable nodes from symbol table in the AST

	end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Program took %f seconds to execute\n", cpu_time_used);
	return 0;
}
