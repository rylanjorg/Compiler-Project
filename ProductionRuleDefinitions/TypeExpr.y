
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

