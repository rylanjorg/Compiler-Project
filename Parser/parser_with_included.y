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

Block: LCBRA BlockList ReturnStmt RCBRA { $$ = createNode($2,$3, NODE_BLOCK); };
 
BlockNoReturn: LCBRA BlockList RCBRA { $$ = createNode($2,NULL, NODE_BLOCK_NO_RETURN); };
 
BlockList: %empty
{
	$$ = NULL;
}
| BlockList StmtList
{
	$$ = createNode($1,$2, NODE_BLOCKLIST);
}
| BlockList VarDecl
{
	$$ = createNode($1,$2, NODE_BLOCKLIST);
};

ConditionalBlock: LCBRA BlockList RCBRA { $$ = createNode($2,NULL, NODE_BLOCK); };



ConditionalExpression: NonAssignExpr CONDITIONAL_OPERATOR NonAssignExpr
{
	struct ASTNode* conditionExpressionASTNode = createNode($1, $3, NODE_CONDITIONAL_EXPRESSION);
	strcpy(conditionExpressionASTNode->type, $2);
	$$ = conditionExpressionASTNode;
	
};



// Increment the num of conditional blocks within the current scope, 
// this is to instantiate the appropriate number of symbol table nodes for them when the scope is exited

IfBlock: IF LPAR ConditionalExpression RPAR BlockNoReturn
{
    IncrementConditionalBlockCounter();
	//struct ASTNode* ifASTNode = createNode($3, $5, NODE_IF);
    struct ASTNode* branchASTNode = createNode($5, NULL, NODE_BRANCH);
    strcpy(branchASTNode->type, "IF");
	struct ASTNode* ifASTNode = createNode($3,branchASTNode, NODE_IF);
    $$ = ifASTNode;
};
| IF LPAR ConditionalExpression RPAR BlockNoReturn ElseIfList
{
    IncrementConditionalBlockCounter();
    struct ASTNode* branchASTNode = createNode($5, $6, NODE_BRANCH);
    strcpy(branchASTNode->type, "IF");
	struct ASTNode* ifASTNode = createNode($3,branchASTNode, NODE_IF);
    $$ = ifASTNode;
};

ElseIfList: %empty
{
    $$ = NULL;
}
| ELSE BlockNoReturn
{
    // Handle the ELSE part within the ElseIfBlock
    IncrementConditionalBlockCounter();
    struct ASTNode* branchASTNode = createNode($2, NULL, NODE_BRANCH);
    strcpy(branchASTNode->type, "ELSE");
    $$ = branchASTNode;
}
| ElseIfBlock ElseIfList 
{
    // Handle the ELSE part within the ElseIfBlock
    IncrementConditionalBlockCounter();
    struct ASTNode* branchASTNode = createNode($1, $2, NODE_BRANCHLIST);
    strcpy(branchASTNode->type, "ELSEIF");
    $$ = branchASTNode;
};


ElseIfBlock: ELSE IF LPAR ConditionalExpression RPAR BlockNoReturn
{
    //$$ = appendElseIfNode($1, $5, $7);

    IncrementConditionalBlockCounter();
    struct ASTNode* branchASTNode = createNode($4, $6, NODE_BRANCH);
    strcpy(branchASTNode->type, "ELSEIF");
	//struct ASTNode* ifASTNode = createNode($3,branchASTNode, NODE_IF);
    $$ = branchASTNode;
};



/*

ElseIfBlock: ELSE IF LPAR ConditionalExpression RPAR BlockNoReturn
{
    //$$ = createElseIfNode($4, $6);
    IncrementConditionalBlockCounter();
    //struct ASTNode* branchASTNode = createNode($1, $7, NODE_BRANCH);
	//struct ASTNode* ifASTNode = createNode($3,branchASTNode, NODE_IF);
    $$ = $6;
}
| ElseIfBlock ELSE IF LPAR ConditionalExpression RPAR BlockNoReturn
{
    //$$ = appendElseIfNode($1, $5, $7);

    IncrementConditionalBlockCounter();
    struct ASTNode* branchASTNode = createNode($1, $7, NODE_BRANCH);
    strcpy(branchASTNode->type, "ELSE IF");
	//struct ASTNode* ifASTNode = createNode($3,branchASTNode, NODE_IF);
    $$ = branchASTNode;
}
| ElseIfBlock ELSE BlockNoReturn
{
    // Handle the ELSE part within the ElseIfBlock
    IncrementConditionalBlockCounter();
    struct ASTNode* branchASTNode = createNode($1, $3, NODE_BRANCH);
    strcpy(branchASTNode->type, "ELSE");
    $$ = branchASTNode;
};

*/

// Increment the num of conditional blocks within the current scope, 
// this is to instantiate the appropriate number of symbol table nodes for them when the scope is exited

WhileBlock: WHILE LPAR ConditionalExpression RPAR BlockNoReturn
{
    IncrementConditionalBlockCounter();
    struct ASTNode* branchASTNode = createNode($5, NULL, NODE_BRANCH);
    strcpy(branchASTNode->type, "WHILE");
	struct ASTNode* whileASTNode = createNode($3,  branchASTNode, NODE_WHILE);
    $$ = whileASTNode;



    char itemKind[8];
	strcpy(itemKind, "WHILE");

	char itemType[8];
	strcpy(itemType, "Loop");

    char itemName[50];
	strcpy(itemType, "while");
	//char identifierType[8];
	//strcpy(identifierType, "N/A");

	

    struct Instruction* whileInstruction = CreateInstruction(itemName, itemKind, itemType, WHILEBLOCK, 3);

    SetInstructionASTNodeRef(whileInstruction, whileASTNode, 0);
	SetInstructionASTNodeRef(whileInstruction, $3, 1);
	SetInstructionASTNodeRef(whileInstruction, $5, 2);
	
	// ------------- Then set the child node --------------

	//$3->node_type == ConditionalExpression
	//$3->node_type == NODE_FUNCTION_CALL
	// Both types of nodes are set to the first index of the instruction array
	

	PushFuncInstruction(whileInstruction);

}
| UNROLL WHILE LPAR ConditionalExpression RPAR BlockNoReturn
{
    IncrementConditionalBlockCounter();
    struct ASTNode* branchASTNode = createNode($6, NULL, NODE_BRANCH);
    strcpy(branchASTNode->type, "WHILE");
	struct ASTNode* whileASTNode = createNode($4,  branchASTNode, NODE_WHILE);
    $$ = whileASTNode;
};

ReturnStmt: RETURN NonAssignExpr SEMICOLON
{
	printf(ANSI_COLOR_BRIGHT_GREEN "Recognized Rule: RETURN\n" ANSI_COLOR_RESET);
    $$ = createNode(NULL, $2, NODE_RETURN); // Create a new AST node for RETURN
};

// summary:
// 1. Check whether the function exists in the glboal symbol table
// 2. If the function is not in the symbol table, add it to the symbol table and process the function scope.
// 3. AstNode declaration
// 4. Item kind declaration
// 5. Add the function to the global symbol table, then resolve the function scope for its instructions
// 6. Create a new symbol table for the function scope
// 7. Display the symbol table
// 8. Resolve the function scope

FuncDecl: TYPE IDENTIFIER LPAR ParamDeclList RPAR Block
{
	printf(ANSI_COLOR_BRIGHT_YELLOW "\n #####- Looking through symbol table for identifier '%s' -##### \n" ANSI_COLOR_RESET, $2);
	int inSymTab = CheckForIdentifierInSymbTab(&head, $2, globScope, false);

	if(inSymTab == 0)
	{
		printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: declaration for global function '%s'\n" ANSI_COLOR_RESET, $2);
		struct ASTNode* funcDecl = createFunctionDeclarationNode($4, $6, $2, $1);
		funcDecl->funcScope = $2;
		$$ = funcDecl;

		char itemKind[8];
		strcpy(itemKind, "Func");

		addItemToGlobalSymTab(head, $2, itemKind, $1, funcDecl); 
		insertSymbolTable(&head, $2, FunctionTable, numLocalConditionalBlocks);


		
		ResetConditionalBlockCounter();


		ShowAllSymTabs(head);

		
		if(strcmp($2, "main") == 0)
		{
			funcDecl->node_type = NODE_MAIN_DECL;
			ResolveFunctionScope($2, $$);
		}
		else
		{
			CopyCurrentInstructionStackToSymbolTable($2);
		}
	}
	else
    {
        printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Function %s is already in the symbol table\n"ANSI_COLOR_RESET, $2);
        exit(1);
    }

};

ParamDeclList: %empty
{
	$$ = NULL;
}
| ParamDecl
{
    $$ = createNode($1, NULL, NODE_PARAM_DECL_LIST);
}
| ParamDecl COMMA ParamDeclList
{
    $$ = createNode($1, $3,NODE_PARAM_DECL_LIST);
};

ParamDecl: TYPE IDENTIFIER
{


	printf(ANSI_COLOR_BRIGHT_GREEN "\nRECOGNIZED RULE: ParamDecl for function -> param IDENTIFIER '%s' of TYPE '%s'\n\n" ANSI_COLOR_RESET, $2, $1);

	char tempValue[50];
	strcpy(tempValue, "Temp NULL");

	struct ASTNode* constantNode = createConstantNode(tempValue, NULL_TYPE);
    struct ASTNode* paramDeclNode = createVariableDeclarationNode($2, $1);
	struct ASTNode* assignmentNode = createNode(paramDeclNode, constantNode, NODE_ASSIGNMENT);
    
	// Add the item to stack to process once the root function identifier is found.
	char itemKind[8];
	strcpy(itemKind, "Param");


	struct Instruction* funcInstruction = CreateInstruction($2, itemKind, $1, PARAM_DECLARATION, 1);
	SetInstructionASTNodeRef(funcInstruction, assignmentNode, 0);



	PushFuncInstruction(funcInstruction);
	$$ = assignmentNode;
};

ParamList: %empty
{
	$$ = NULL;
}
| Param
{
    $$ = createNode($1, NULL, NODE_PARAM_LIST);
}
| Param COMMA ParamList
{ 
    $$ = createNode($1, $3, NODE_PARAM_LIST);
};


Param: IDENTIFIER
{
	/// summary:
	///
	/// The IDENTIFIER for a Param must be previously declared in the symbol table for the given scope. 
	/// Because the SEMANTIC CHECK for requires the scope to be known, it must be processed later.
	///
	/// So, what type of node should a Param be? 
	/// Based on the previous definition, it will always be a reference to a variable in the symbol table.
	/// 
	/// - There is some IDENTIFIER in the symbol table with the same name as the IDENTIFIER in the param, and it has an assignment node.
	///
	/// - This ASTNode will simply be a reference to the assignment node of the variable in the symbol table.
	///
	/// - The reference from this ASTNode will be assigned to the corresponding ASTNode in the function declaration as the scope is resolved.
	///
	/// Based on this outline, we can initalize this node to an essentially an assignment node with a NULL left node, and a NULL right node.
	/// To avoid confusion I will use NODE_PARAM for this node type, although it represents the same structure as the assignment node. 

	printf(ANSI_COLOR_BRIGHT_GREEN "\nRECOGNIZED RULE: Param for function -> param IDENTIFIER '%s' \n\n" ANSI_COLOR_RESET,  $1);

	struct ASTNode* paramDeclNode = createNode(NULL, NULL, NODE_PARAM);
	
	char itemKind[8];
	strcpy(itemKind, "ParamIn");

	

	// Add the item to stack to process once the root function identifier is found.
	struct Instruction* funcInstruction = CreateInstruction($1, itemKind, $1, param_IDENTIFIER, 1);
	SetInstructionASTNodeRef(funcInstruction, paramDeclNode, 0);


	PushFuncInstruction(funcInstruction);

	$$ = paramDeclNode;
};


GlobalBlockList: %empty
{
	$$ = NULL;
}
| GlobalBlockList GlobVarDecl
{
	$$ = createNode($1,$2, NODE_BLOCKLIST);
}
| GlobalBlockList FuncDecl
{
	$$ = createNode($1,$2, NODE_BLOCKLIST);
};


GlobVarDecl: TYPE IDENTIFIER SEMICOLON	
{ 
	printf(ANSI_COLOR_BRIGHT_YELLOW "\n #####- Looking through symbol table for identifier '%s' -##### \n" ANSI_COLOR_RESET, $2);

	int inSymTab = CheckForIdentifierInSymbTab(head, $2,globScope, false);
								
	if (inSymTab == 0)
	{
		printf(ANSI_COLOR_BRIGHT_YELLOW "\n #####- identifier '%s' was not found in the global symbol table -##### \n" ANSI_COLOR_RESET, $2);
		printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: declaration for global variable '%s'\n" ANSI_COLOR_RESET, $2); 

		struct ASTNode* ver_decl = createVariableDeclarationNode($2, $1);
		ver_decl->isInitialized = false; // Initialize to false


		/// summary:
		/// Add the item to glob Symbol table since the scope is known at this time.
		char itemKind[8];
		strcpy(itemKind, "Var");
		
		addItemToGlobalSymTab(head, $2, itemKind, $1, ver_decl);
		$$ = ver_decl;
	}
	else
	{
		printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Var %s is already in the global symbol table\n"ANSI_COLOR_RESET, $2);
		exit(1);
	}

	ShowAllSymTabs(head);
	

}
| GlobalTypeExpr SEMICOLON
{
	$$ = createNode(NULL, $1, NODE_GLOB_VAR_BLOCKLIST);
	printStackContents();  
}
| TYPE IDENTIFIER LBRACKET INT_LITERAL RBRACKET SEMICOLON
{
	int num = $4;
	printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: declaration for global array of size : %d\n" ANSI_COLOR_RESET, num); 

	struct ASTNode* arrayDeclNode = createArrayDeclarationNode($2, $1, num);
	$$ = arrayDeclNode;
};




GlobalTypeExpr: TYPE IDENTIFIER {}
| TYPE IDENTIFIER EQ GlobNonAssignExpr   
{ 
	printf(ANSI_COLOR_BRIGHT_YELLOW "\n #####- Looking through symbol table for identifier '%s' -##### \n" ANSI_COLOR_RESET, $2);

    int inSymTab = CheckForIdentifierInSymbTab(head, $2,globScope, false);

    if (inSymTab == 0)
    {
        printf(ANSI_COLOR_BRIGHT_YELLOW "\n #####- identifier '%s' was not found in the global symbol table -##### \n" ANSI_COLOR_RESET, $2);
        printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: declaration for global variable '%s'\n" ANSI_COLOR_RESET, $2); 

        struct ASTNode* ver_decl = createVariableDeclarationNode($2, $1);

        /// summary:
        /// Add the item to glob Symbol table since the scope is known at this time.
        char itemKind[8];
        strcpy(itemKind, "Var");

        // Mark the variable as initialized
        int update_eval = TryIncrementVarNumRef(head,$2,globScope);
        ver_decl->isInitialized = true;

		addItemToGlobalSymTab(head, $2, itemKind, $1, ver_decl);

		if($4->node_type == NODE_CONSTANT)
		{
			printf(ANSI_COLOR_YELLOW "\n%s $4->node_type == NODE_CONSTANT, $4->value == '%s'\n" ANSI_COLOR_RESET,"           ", $4->value); 

			char str[50];	
			strcpy(str, $4->value);

			struct ASTNode* assignmentNode = createNode(ver_decl, $4, NODE_ASSIGNMENT);

			printf(ANSI_COLOR_GREY "%s--> Created Assignment Node with pointer '%p', right value '%s', and left varDeclNode (value: '%s' type: '%s') \n"ANSI_COLOR_RESET, "           ", (void*)assignmentNode, $4->value, ver_decl->value, ver_decl->type);
        	int updated_assin_ref = TryUpdateAssignmentNodeRef(head, $2, globScope, assignmentNode);
			if(updated_assin_ref == 0) 	
			{
				printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Could not update Assignment node ref because it could not be found in the symbol table with scope '%s'\n"ANSI_COLOR_RESET, globScope);
				exit(1);
			}
			int updated_var = TryUpdateVarValue(head, $2, globScope, str);

			$$ = assignmentNode;
		}
		else
		{
			printf(ANSI_COLOR_BRIGHT_RED "\nWARNING: $4->node_type != NODE_CONSTANT" ANSI_COLOR_RESET);
			$$ = ver_decl;
		}
    }
    else
    {
        printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Var %s is already in the global symbol table\n"ANSI_COLOR_RESET, $2);
        exit(1);
    }

    ShowAllSymTabs(head);
	
}
| TYPE IDENTIFIER LBRACKET INT_LITERAL RBRACKET EQ GlobNonAssignExpr    
{
	push("IDENTIFIER LBRACKET NUMBER RBRACKET EQ");
	printStackContents(); 
};




VarDecl: TYPE IDENTIFIER SEMICOLON 
{ 
	/*
	// Create ASTNode* here now, assuming the code passes all semantic checks. Then, once the scope is known go and process semantic checks.
	struct ASTNode* varDeclNode = createVariableDeclarationNode($2, $1);

	// Initialize to false
	varDeclNode->isInitialized = false; 

	$$ = varDeclNode;
	
	char itemKind[8];
	strcpy(itemKind, "Var");

	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction($2, itemKind, $1, varDeclNode, VAR_DECLARATION);
	PushFuncInstruction(funcInstruction);
	*/


	// ----------- Same as Var Decl Rule ------------

	// Create ASTNode* here now, assuming the code passes all semantic checks. Then, once the scope is known go and process semantic checks.
	struct ASTNode* varDeclNode = createVariableDeclarationNode($2, $1);

	printf(ANSI_COLOR_GREY "\n --> Info: Variable Declaration Node pointer -> '%p'. Variable name: '%s'. Variable type '%s'." ANSI_COLOR_RESET, varDeclNode, $2, $1);
	
	// Initialize to false
	varDeclNode->isInitialized = false; 

	char itemKind[8];
	strcpy(itemKind, "Var");

	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction($2, itemKind, $1, VAR_DECLARATION, 1);
	SetInstructionASTNodeRef(funcInstruction, varDeclNode, 0);

	//TYPE_IDENTIFIER_EQ_NonAssignExpr
	// ------------- Then make the assignment node --------------

	varDeclNode->isInitialized = false; 
	printf(ANSI_COLOR_GREY "\n --> Info: Is Variable '%s' initalized -> '%d." ANSI_COLOR_RESET, $2, varDeclNode->isInitialized);

	char tempValue[50];
	strcpy(tempValue, "null");

	DataType dataType;


	if(strcmp($1, "int") == 0) { dataType = INT; }
	else if(strcmp($1, "float") == 0) { dataType = FLOAT; }
	else if(strcmp($1, "char") == 0) { dataType = CHAR; }
	else { printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Uncregonized data type in VarDecl rule" ANSI_COLOR_RESET); exit(1); }
	

	struct ASTNode* assignment = createNode(varDeclNode, createConstantNode(tempValue, dataType), NODE_ASSIGNMENT);
	printf(ANSI_COLOR_GREY "\n --> Info: Assignment node created. With left pointing to varDeclNode -> '%p' and right pointing to null -> '%p'. Assignment node pointer -> '%p'." ANSI_COLOR_RESET, assignment->left, assignment->right, assignment);

	//printf(ANSI_COLOR_BRIGHT_MAGENTA "\n Setting assignment node for the instruction with value '%s' "ANSI_COLOR_RESET, $4->value);
	SetAssignmentNodeRef(funcInstruction, assignment);
	$$ = assignment;


	//printf(ANSI_COLOR_GREY "\n --> Info: Function instruction pushed to stack. Instruction has pointer -> '%p' as the Assignment node reference, and pointer -> '%p' as the ASTNode reference." ANSI_COLOR_RESET, funcInstruction->assignmentNodeRef, funcInstruction->ast_node_ref);
	PushFuncInstruction(funcInstruction);







}
| TypeExpr SEMICOLON
{
	$$ = createNode(NULL,$1, NODE_BLOCKLIST);
	printStackContents();  
}
| TYPE IDENTIFIER LBRACKET INT_LITERAL RBRACKET SEMICOLON
{
	int index = $4;
	printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: declaration for array of size : %d\n" ANSI_COLOR_RESET, index); 


	char itemKind[8];
	strcpy(itemKind, "Array");

	char tempValue[50];
	strcpy(tempValue, "Temp Null [TYPE IDENTIFIER LBRACKET INT_LITERAL RBRACKET SEMICOLON]");

	struct ASTNode* arrayDeclNode = createArrayDeclarationNode($2, $1, index);

	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction($2, itemKind, $1, ARRAY_DECLARATION, 2);
	

	//char elementStr[50];
	//sprintf(elementStr, "%s[%d]", $2, index);

	// Assignment node for the element
	//struct ASTNode* varDeclNode = createVariableDeclarationNode(elementStr, $1);
	//struct ASTNode* assignment = createNode(varDeclNode, createConstantNode(tempValue, ARRAY_TEMP_ELEMENT), NODE_ASSIGNMENT);

	SetInstructionASTNodeRef(funcInstruction, arrayDeclNode, 0);
	//SetInstructionASTNodeRef(funcInstruction, assignment, 1);

	PushFuncInstruction(funcInstruction);

	$$ = arrayDeclNode;
};
Stmt: SEMICOLON	
{
	printf(ANSI_COLOR_BRIGHT_GREEN " 	RECOGNIZED RULE: stmt -> SEMICOLON "ANSI_COLOR_RESET);
}
| Expr SEMICOLON	
{	
	printf(ANSI_COLOR_BRIGHT_GREEN " 	RECOGNIZED RULE: stmt -> Expr SEMICOLON \n"ANSI_COLOR_RESET);
	if($1 != NULL)
		$$ = createNode(NULL, $1, NODE_STMT);
	else
	{
		$$ = NULL;
	}
}
| IF LPAR ConditionalExpression RPAR ConditionalBlock
{
	printf(ANSI_COLOR_BRIGHT_GREEN " 	RECOGNIZED RULE: stmt -> IF LPAR RPAR Block \n"ANSI_COLOR_RESET);
	//$$ = createNode(NULL, $4, NODE_STMT);
}
| IF LPAR ConditionalExpression RPAR ConditionalBlock ELSE ConditionalBlock
{
	printf(ANSI_COLOR_BRIGHT_GREEN " 	RECOGNIZED RULE: stmt -> IF LPAR RPAR Block ELSE Block \n"ANSI_COLOR_RESET);
	//$$ = createNode($4, $6, NODE_STMT);
}
| WhileBlock
{ 
	printf(ANSI_COLOR_BRIGHT_GREEN " 	RECOGNIZED RULE: While Block \n"ANSI_COLOR_RESET);
}
| IfBlock
{ 
	printf(ANSI_COLOR_BRIGHT_GREEN " 	RECOGNIZED RULE: IF Block \n"ANSI_COLOR_RESET);
};

StmtList: Stmt StmtList	
{ 
	if($1 != NULL) 
		$$ = createNode($1, $2, NODE_STMT);  
	else
		$$ = $2;
}
| Stmt 
{ 
	if($1 != NULL)
		$$ = $1; 
	else
		$$ = NULL; 

};


TypeExpr: TYPE IDENTIFIER {}
| TYPE IDENTIFIER EQ NonAssignExpr    
{ 
	push("TYPE IDENTIFIER EQ");

	// ----------- Same as Var Decl Rule ------------

	// Create ASTNode* here now, assuming the code passes all semantic checks. Then, once the scope is known go and process semantic checks.
	struct ASTNode* varDeclNode = createVariableDeclarationNode($2, $1);

	printf(ANSI_COLOR_GREY "\n --> Info: Variable Declaration Node pointer -> '%p'. Variable name: '%s'. Variable type '%s'." ANSI_COLOR_RESET, varDeclNode, $2, $1);
	
	// Initialize to false
	varDeclNode->isInitialized = false; 

	char itemKind[8];
	strcpy(itemKind, "Var");

	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction($2, itemKind, $1, TYPE_IDENTIFIER_EQ_NonAssignExpr, 3);
	SetInstructionASTNodeRef(funcInstruction, varDeclNode, 0);
	
	// ------------- Then make the assignment node --------------

	varDeclNode->isInitialized = true; 
	SetInstructionASTNodeRef(funcInstruction, $4, 1);
	struct ASTNode* assignment = createNode(varDeclNode, NULL, NODE_ASSIGNMENT);
	printf(ANSI_COLOR_GREY "\n --> Info: Assignment node created. With left pointing to varDeclNode -> '%p' and right pointing to null -> '%p'. Assignment node pointer -> '%p'." ANSI_COLOR_RESET, assignment->left, assignment->right, assignment);
	SetInstructionASTNodeRef(funcInstruction, assignment, 2);


	switch($4->node_type)
	{
		case NODE_FUNCTION_CALL:
			printf(ANSI_COLOR_GREY "\n --> Info: $4->node type is NODE_FUNCTION_CALL. Is Variable '%s' initalized -> '%d." ANSI_COLOR_RESET, $2, varDeclNode->isInitialized);
			struct ASTNode* block = createNode($4, assignment, NODE_BLOCK);
			$$ = block;
			break;
		case NODE_CONSTANT:
			printf(ANSI_COLOR_GREY "\n --> Info: $4->node type is NODE_CONSTANT. Is Variable '%s' initalized -> '%d." ANSI_COLOR_RESET, $2, varDeclNode->isInitialized);
			$$ = assignment;
			break;
		case NODE_BINARY_EXPRESSION:
			printf(ANSI_COLOR_GREY "\n --> Info: $4->node type is NODE_BINARY_EXPRESSION. Is Variable '%s' initalized -> '%d." ANSI_COLOR_RESET, $2, varDeclNode->isInitialized);
			$$ = assignment;
			break;
		default:
			printf(ANSI_COLOR_BRIGHT_RED "\nERROR: Unrecognized node type for NonAssignExpr in rule [TYPE IDENTIFIER EQ NonAssignExpr] where NonAssignExpr type is '%d'." ANSI_COLOR_RESET, $4->node_type);
			exit(1);
			$$ = varDeclNode;
			break;
	}

	
	PushFuncInstruction(funcInstruction);
	
}
| TYPE IDENTIFIER LBRACKET INT_LITERAL RBRACKET EQ NonAssignExpr    
{

}



Expr: IDENTIFIER {}
| IDENTIFIER EQ NonAssignExpr    
{ 
	// Create the instruction to be processed once the function scope is known. 
	// We dont know the scope yet so we cant get the varaible referemce at this point, so
	// it is set to NULL.
	
	char itemKind[8];
	strcpy(itemKind, "Stmt");

	char itemType[8];
	strcpy(itemType, "Assign");

	
	char identifierType[8];
	strcpy(identifierType, "Temp NULL");

	

	struct Instruction* funcInstruction = CreateInstruction($1, itemKind, itemType, IDENTIFIER_EQ, 2);

	struct ASTNode* varDeclNode = createVariableDeclarationNode($1, identifierType);
	struct ASTNode* assignmentNode = createNode(varDeclNode, $3, NODE_ASSIGNMENT);
	SetInstructionASTNodeRef(funcInstruction, assignmentNode, 0);
	SetInstructionASTNodeRef(funcInstruction, $3, 1);
	
	// ------------- Then set the child node --------------

	//$3->node_type == NODE_CONSTANT
	//$3->node_type == NODE_FUNCTION_CALL
	// Both types of nodes are set to the first index of the instruction array
	

	PushFuncInstruction(funcInstruction);

	// create an assignment node for potential assignment. It is easier to just remove assignment node then to add it later if it already has an assignment in the current scope.
	// have to set the left node to NULL, given we dont know the scope yet and cant get the IDENTIFIER's "var" Node.
	


	//$$ = NULL;
	$$ = assignmentNode;


	printStackContents();  

}
|	IDENTIFIER LBRACKET INT_LITERAL RBRACKET EQ NonAssignExpr    
{
/*
	char itemKind[8];
	strcpy(itemKind, "Stmt");

	char itemType[8];
	strcpy(itemType, "Assign");
	
	//struct Instruction* funcInstruction = CreateInstruction($1, itemKind, itemType, ARRAY_IDENTIFIER_EQ, 3);
	//SetInstructionASTNodeRef(funcInstruction, NULL, 0);

	
	if($6->node_type == NODE_CONSTANT)
	{
		SetInstructionASTNodeRef(funcInstruction, $3, 1);
	}

	
	PushFuncInstruction(funcInstruction);

	// create an assignment node for potential assignment. It is easier to just remove assignment node then to add it later if it already has an assignment in the current scope.
	// have to set the left node to NULL, given we dont know the scope yet and cant get the IDENTIFIER's "var" Node.


	char index[50];
	sprintf(index, "%d", $3);

	struct ASTNode* assignmentNode = createArrayAssignmentNode(NULL, $3, index);
	SetInstructionASTNodeRef(funcInstruction, assignmentNode, 2);

	$$ = assignmentNode;
*/



	int index = $3;
	printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: assignment for array at index : %d\n" ANSI_COLOR_RESET, index); 


	char itemKind[8];
	strcpy(itemKind, "ArrElem");
	
	char itemType[8];
	sprintf(itemType, "%d", index);

	char tempValue[50];
	strcpy(tempValue, "Temp Null [TYPE IDENTIFIER LBRACKET INT_LITERAL RBRACKET SEMICOLON]");
	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction($1, itemKind, itemType, ARRAY_IDENTIFIER_EQ, 3);
	
	char elementStr[50];
	sprintf(elementStr, "%s[%d]", $1, index);

	// Assignment node for the element
	struct ASTNode* varDeclNode = createVariableDeclarationNode(elementStr, $1);
	struct ASTNode* assignment = createNode(varDeclNode, createConstantNode(tempValue, ARRAY_TEMP_ELEMENT), NODE_ASSIGNMENT);

	SetInstructionASTNodeRef(funcInstruction, assignment, 0);
	SetInstructionASTNodeRef(funcInstruction, $6, 1);

	PushFuncInstruction(funcInstruction);

	$$ = assignment;


}
| WRITE IDENTIFIER      
{ 
	printf("\n RECOGNIZED RULE: WRITE statement\n");

	// Create the instruction to be processed once the function scope is known.
	char itemKind[8];
	strcpy(itemKind, "keyword");

	char itemType[8];
	strcpy(itemType, "Write");

	struct ASTNode* writeNode = createNode(NULL, NULL, NODE_WRITE);

	struct Instruction* funcInstruction = CreateInstruction($2, itemKind, itemType, WRITE_IDENTIFIER, 1);
	SetInstructionASTNodeRef(funcInstruction, writeNode, 0);

	PushFuncInstruction(funcInstruction);
	
	$$ = writeNode;

	printStackContents(); 
};





NonAssignExpr:IDENTIFIER
{
	// Schedule to resolve the identifier scope later. Will just need to search to see if it exists in the  current symbol table scope.

	char itemKind[8];
	strcpy(itemKind, "Ident");

	char itemType[8];
	strcpy(itemType, "NAsEx");

	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction($1, itemKind, itemType, NonAssignExpr_IDENTIFIER, 2);
	SetInstructionASTNodeRef(funcInstruction, NULL, 0);
	
	char tempValue[50];
	strcpy(tempValue, "null");

	
	struct ASTNode* constantNode = createConstantNode(tempValue, NULL_TYPE);
	struct ASTNode* assignmentNode = createNode(NULL, constantNode , NODE_ASSIGNMENT);

	SetInstructionASTNodeRef(funcInstruction, assignmentNode, 1);

	PushFuncInstruction(funcInstruction);
	$$ = assignmentNode;
}
| IDENTIFIER LPAR ParamList RPAR
{
	/// summary:
	/// So the ParamList is a list of ASTNodes that are references to the assignment nodes in the symbol table. This is processed in the ResolveFunctionScope function.
	/// So since the ParamList gets processed first, we have references to each of the params we are passing to the function.
	/// 
	/// Goal Outline:
	///
	/// - Use the ParamList to get the references to the param assignment nodes and assign each to the corresponding decl params in the function. [This handles passing the data to the function]
	///	     - SEMANTIC CHECKS: 
	///			- variable types need to match
	///			- the number of parameters need to match
	///	
	///	Im not sure how to handle the return, mainly just because idk how to handle optimizations. To outline the problem:
	///	
	///	- The return Node already holds the return value due to optimizations. If all of the code in the function can be marked as dead, 
	///   then the function doesn't need to made at all in the IRCode and MIPS code. But, if there are some dependencies, then the function needs to be made.
	///   The return node can still be optimized by constant propogation and constant folding, but the non dead code cant be removed. For example:
	///   	
	///	  FuncExample(int a, int b)
	///   {
	///		int k = a + b;
	///		int q = 5;
	///		write q;
	///		q = k;
	///		return k;
	///	  }



	printf(ANSI_COLOR_BRIGHT_GREEN "Recognized Rule: Function Expr\n" ANSI_COLOR_RESET);
	printAST($3,0,0);



	char itemKind[8];
	strcpy(itemKind, "");

	char itemType[8];
	strcpy(itemType, "Func");

	char tempValue[50];
	strcpy(tempValue, "Temp Null");

	int numParams = CountInstances($3, NODE_PARAM);

	struct ASTNode* returnNode = createFunctionCallNode(NULL,createConstantNode(tempValue, NULL_TYPE), $1, numParams);

	if(returnNode == NULL) 
	{ 
		printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: failed to create ASTNode for function call to '%s' \n"ANSI_COLOR_RESET, $1);
		exit(1);
	}
	else if(returnNode != NULL)
	{
		printf(ANSI_COLOR_BRIGHT_GREEN "\nRECOGNIZED RULE: Function call to '%s'. Successfully created ASTNode '%p' \n"ANSI_COLOR_RESET, $1, returnNode);
	}

	

	//struct ASTNode* constantNode = createConstantNode(actualValue);
	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction($1, itemKind, itemType, IDENTIFIER_LPAR_ParamList_RPAR, 2);
	SetInstructionASTNodeRef(funcInstruction, returnNode, 0);

	//struct Instruction* funcInstruction = CreateInstruction($1, itemKind, itemType, functionCallNode, IDENTIFIER_LPAR_ParamList_RPAR);
	
	SetInstructionASTNodeRef(funcInstruction, $3, 1);
	PushFuncInstruction(funcInstruction);
	
	$$ = returnNode;
	//$$ = $3;

}
| IDENTIFIER OPERATOR NonAssignExpr 
{

	struct ASTNode* returnNode = NULL;

	char itemKind[8];
	strcpy(itemKind, $2);

	char itemType[8];
	strcpy(itemType, "IdOp");

	char tempValue[50];
	strcpy(tempValue, "Temp Null");
	
	// Create a Binary expression node for the IDENTIFIER Operatore NonAssignExpr rule. 
	// If the NonAssignExpr evaluates to a constant and the IDENTIFIER in the "IDENTIFIER OPERATOR NonAssignExpr"
	// is not of type param, then just create a constant node and pass it to the binary expression node.
	// Since we can't evalueate whether an IDENTIFIER is a param until the IDENTIFIER is in the symboltable, we
	// have to wait until the function scope is known to process whether the constant propogation can occur. 
	// So to summarize:
	// - Create Binary expression node 
	// - then during the ResolveFunctionScope function, check whether the IDENTIFIER is a param. If it is, then 
	// dont constant fold, else constant fold.

	/*if($3->node_type == NODE_CONSTANT) 
	{ 
		returnNode = createConstantNode(tempValue, NULL_TYPE); 
	}
	else 
	{ */
	returnNode = createBinaryExpressionNode($2, createConstantNode(tempValue, NULL_TYPE), $3); 
	//}

	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction($1, itemKind, itemType, IDENTIFIER_OPERATOR_NonAssignExpr, 3);
	SetInstructionASTNodeRef(funcInstruction, returnNode, 0);

	// Instead of $3->value, pass the node created for $3 to the instruction
	SetInstructionASTNodeRef(funcInstruction, $3, 1);

	printf(ANSI_COLOR_BRIGHT_GREEN "\n SetInstructionChild(funcInstruction, returnNode) \n" ANSI_COLOR_RESET);
	PushFuncInstruction(funcInstruction);

	$$ = returnNode;

}
| INT_LITERAL OPERATOR NonAssignExpr 
{
	struct ASTNode* returnNode = NULL;


	char c_str[50];
	sprintf(c_str, "%d", $1);

	// Create a constant node for the left number
	struct ASTNode* constantNode = createConstantNode(c_str, INT);

	
	char itemKind[8];
	strcpy(itemKind, $2);

	char itemType[8];
	strcpy(itemType, "NumOp");

	char tempValue[50];
	strcpy(tempValue, "null");

	/*if($3->node_type == NODE_CONSTANT) 
	{ 
		returnNode = createConstantNode(tempValue, INT); 
	}
	else 
	{ 
		returnNode = createBinaryExpressionNode($2, createConstantNode(tempValue, INT), $3); 
	}*/

	returnNode = createBinaryExpressionNode($2, createConstantNode(tempValue, INT), $3); 

	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction(c_str, itemKind, itemType, NUMBER_OPERATOR_NonAssignExpr, 3);
	SetInstructionASTNodeRef(funcInstruction, returnNode, 0);

	// Instead of $3->value, pass the node created for $3 to the instruction
	SetInstructionASTNodeRef(funcInstruction, $3, 1);

	// Set the 2nd index to the constant node
	SetInstructionASTNodeRef(funcInstruction, constantNode, 2);

	printf(ANSI_COLOR_BRIGHT_GREEN "\n SetInstructionChild(funcInstruction, returnNode) \n" ANSI_COLOR_RESET);
	PushFuncInstruction(funcInstruction);

	$$ = returnNode;
} 
| FLOAT_LITERAL OPERATOR NonAssignExpr 
{
	struct ASTNode* returnNode = NULL;
	
	char c_str[50];
	sprintf(c_str, "%f", $1);

	// Create a constant node for the left number
	struct ASTNode* constantNode = createConstantNode(c_str, FLOAT);



	char itemKind[8];
	strcpy(itemKind, $2);

	char itemType[8];
	strcpy(itemType, "NumOp");


	char tempValue[50];
	strcpy(tempValue, "null");

	/*
	if($3->node_type == NODE_CONSTANT) 
	{ 
		returnNode = createConstantNode(tempValue, FLOAT); 
	}
	else 
	{ 
		returnNode = createBinaryExpressionNode($2, createConstantNode(tempValue, FLOAT), $3); 
	}*/

	returnNode = createBinaryExpressionNode($2, createConstantNode(tempValue, FLOAT), $3); 

	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction(c_str, itemKind, itemType, NUMBER_OPERATOR_NonAssignExpr, 3);
	SetInstructionASTNodeRef(funcInstruction, returnNode, 0);

	// Instead of $3->value, pass the node created for $3 to the instruction
	SetInstructionASTNodeRef(funcInstruction, $3, 1);

	// Set the 2nd index to the constant node
	SetInstructionASTNodeRef(funcInstruction, constantNode, 2);



	printf(ANSI_COLOR_BRIGHT_GREEN "\n SetInstructionChild(funcInstruction, returnNode) \n" ANSI_COLOR_RESET);
	PushFuncInstruction(funcInstruction);

	$$ = returnNode;
} 
| IDENTIFIER LBRACKET INT_LITERAL RBRACKET OPERATOR NonAssignExpr
{
	struct ASTNode* returnNode = NULL;
	
	char arrayIdentifier[50];
	strcpy(arrayIdentifier,$1);


	



	char itemKind[8];
	strcpy(itemKind, $5);

	char itemType[8];
	sprintf(itemType, "%d", $3);

	char tempValue[50];
	strcpy(tempValue, "null");

	// Create a constant node for the left number
	struct ASTNode* constantNode = createConstantNode(tempValue, ARRAY_TEMP_ELEMENT);

	returnNode = createBinaryExpressionNode($5, createConstantNode(tempValue, ARRAY_TEMP_ELEMENT), $6); 

	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction(arrayIdentifier, itemKind, itemType, NUMBER_OPERATOR_NonAssignExpr, 3);
	SetInstructionASTNodeRef(funcInstruction, returnNode, 0);

	// Instead of $6->value, pass the node created for $6 to the instruction
	SetInstructionASTNodeRef(funcInstruction, $6, 1);

	// Set the 2nd index to the constant node
	SetInstructionASTNodeRef(funcInstruction, constantNode, 2);

	PushFuncInstruction(funcInstruction);

	$$ = returnNode;
}
| FLOAT_LITERAL              
{ 
	char str[50];
	sprintf(str, "%f", $1); 

	printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: FLOAT_LITERAL -> '%s'\n" ANSI_COLOR_RESET, str);
	
	$$ = createConstantNode(str, FLOAT);
};  
| INT_LITERAL                
{ 
	char str[50];
	sprintf(str, "%d", $1); 

	printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: INT_LITERAL -> '%s'\n" ANSI_COLOR_RESET, str);

	$$ = createConstantNode(str, INT);
}
| CHAR_LITERAL
{
	char charValue[50];

	// Copy the character to the array and null terminate the string.
	charValue[0] = $1;
	charValue[1] = '\0';

	printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: CHAR_LITERAL -> %s \n" ANSI_COLOR_RESET, charValue);

	$$ = createConstantNode(charValue, CHAR);
}
| IDENTIFIER LBRACKET INT_LITERAL RBRACKET
{
	char tempValue[50];
	strcpy(tempValue, "Temp Null [IDENTIFIER LBRACKET INT_LITERAL RBRACKET]");

	printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: IDENTIFIER LBRACKET INT_LITERAL RBRACKET \n" ANSI_COLOR_RESET);

	$$ = createConstantNode(tempValue, ARRAY_TEMP_ELEMENT);
};







GlobNonAssignExpr:IDENTIFIER
{
	push("IDENTIFIER");
	struct ASTNode* identifier_node = GetASTNodeRef(head, $1,globScope, false, "");
	int update_eval = TryIncrementVarNumRef(head, $1,globScope);
	printf(ANSI_COLOR_BRIGHT_MAGENTA "\n IDENTIFIER %s pointer address: %p"ANSI_COLOR_RESET, $1,(void*)identifier_node);


	if(identifier_node != NULL)
	{
		// -------------------------------- Manage AST Node Generation --------------------------------
		//$$ = identifier_node;

		// Try and get the current value from the symbol table
		char* symtable_value = TryGetVarValue(head,$1, globScope);

		// If we got the value from the symbol table replace the identifier with its value
		if (symtable_value != NULL) 
		{
			// Replace the identifier with its value
			char actualValue[50];
			strcpy(actualValue, symtable_value);
			printf(ANSI_COLOR_GREEN "\n Constant Propogation: Setting '%s' to '%s' " ANSI_COLOR_RESET, $1, actualValue);
			
			int update_eval = TryDecrementVarNumRef(head, $1,globScope);


			$$ = createConstantNode(actualValue, NULL_TYPE);
		}
		else
		{
			$$ = identifier_node;
		}

		// -------------------------------- Manage MIPS Code Generation -------------------------------- 
	}
	else
    {
        printf(ANSI_COLOR_BRIGHT_RED"\n SEMANTIC ERROR: identifier '%s' not found\n"ANSI_COLOR_RESET, $1);
        exit(1);
    }
		
}
| IDENTIFIER OPERATOR GlobNonAssignExpr
{
	push("IDENTIFIER OPERATOR");
	struct ASTNode* identifier_node = GetASTNodeRef(head, $1,globScope, false, "");
	int update_eval = TryIncrementVarNumRef(head, $1,globScope);
	printf(ANSI_COLOR_BRIGHT_MAGENTA "\n IDENTIFIER %s pointer address: %p"ANSI_COLOR_RESET, $1,(void*)identifier_node);


	if(identifier_node != NULL)
	{
		// -------------------------------- Constant propagation Optimization --------------------------------
		
		// Try and get the current value from the symbol table
		char* symtable_value = TryGetVarValue(head, $1, globScope);

		// If we got the value from the symbol table replace the identifier with its value
		if (symtable_value != NULL) 
		{
			// Replace the identifier with its value
			char actualValue[50];
			strcpy(actualValue, symtable_value);
			printf(ANSI_COLOR_GREEN "\n Constant Propogation: Setting '%s' to '%s' " ANSI_COLOR_RESET, $1, actualValue);
			
			int update_eval = TryDecrementVarNumRef(head, $1,globScope);

			// -------------------------------- Constant folding Optimization --------------------------------

			if($3->node_type == NODE_CONSTANT)
			{
				char fc_str[50];	

				int folded_constant;

				if(strcmp($2, "+") == 0)
				{
					folded_constant = atoi(actualValue) + atoi($3->value);
				}
				else if(strcmp($2, "-") == 0)
				{
					folded_constant = atoi(actualValue) - atoi($3->value);
				}
				else if(strcmp($2, "*") == 0)
				{
					folded_constant = atoi(actualValue) * atoi($3->value);
				}
				else if(strcmp($2, "/") == 0)
				{
					folded_constant = atoi(actualValue) / atoi($3->value);
				}

				sprintf(fc_str, "%d", folded_constant);

				printf(ANSI_COLOR_GREEN "\n Constant folded: '%d' %s '%d' = '%d' " ANSI_COLOR_RESET, atoi(actualValue), $2, atoi($3->value), folded_constant);

			

				$$ = createConstantNode(fc_str, NULL_TYPE);
			}
			else
			{
				$$ = createBinaryExpressionNode($2, createConstantNode(actualValue, NULL_TYPE), $3);
			}
			// -------------------------------- -------------------------------- --------------------------------

		}
		else
		{
			$$ = createBinaryExpressionNode($2, identifier_node, $3);
		}

		// -------------------------------- Manage MIPS Code Generation -------------------------------- 

	}
	else
	{
		printf(ANSI_COLOR_BRIGHT_RED"\n SEMANTIC ERROR: identifier '%s' not found\n"ANSI_COLOR_RESET, $1);
		exit(1);
	}
}
| INT_LITERAL OPERATOR GlobNonAssignExpr
{

	if($3->node_type == NODE_CONSTANT)
	{
		char fc_str[50];	

		int folded_constant;

		if(strcmp($2, "+") == 0)
		{
			folded_constant = $1 + atoi($3->value);
		}
		else if(strcmp($2, "-") == 0)
		{
			folded_constant = $1 - atoi($3->value) ;
		}
		else if(strcmp($2, "*") == 0)
		{
			folded_constant = $1 * atoi($3->value);
		}
		else if(strcmp($2, "/") == 0)
		{
			folded_constant = $1 / atoi($3->value);
		}
		


		sprintf(fc_str, "%d", folded_constant);

		printf(ANSI_COLOR_GREEN "\n Constant folded: '%d' %s '%d' = '%d' " ANSI_COLOR_RESET, $1, $2, atoi($3->value), folded_constant);
		
		$$ = createConstantNode(fc_str, INT);
	}
	else
	{
		char c_str[50];
		sprintf(c_str, "%d", $1);
		$$ = createBinaryExpressionNode($2, createConstantNode(c_str, INT), $3);	
	}

} 
| INT_LITERAL                
{ 
	char str[50];
	sprintf(str, "%d", $1); 

	$$ = createConstantNode(str, INT);
	
};  
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
