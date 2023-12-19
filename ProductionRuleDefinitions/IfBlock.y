
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
