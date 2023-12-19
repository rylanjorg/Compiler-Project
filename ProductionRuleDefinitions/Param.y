
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
