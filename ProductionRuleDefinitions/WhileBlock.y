
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