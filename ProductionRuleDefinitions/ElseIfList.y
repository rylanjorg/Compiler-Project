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

