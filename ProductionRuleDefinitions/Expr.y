
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



