
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