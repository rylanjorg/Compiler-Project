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
