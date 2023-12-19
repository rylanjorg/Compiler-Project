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