
ConditionalExpression: NonAssignExpr CONDITIONAL_OPERATOR NonAssignExpr
{
	struct ASTNode* conditionExpressionASTNode = createNode($1, $3, NODE_CONDITIONAL_EXPRESSION);
	strcpy(conditionExpressionASTNode->type, $2);
	$$ = conditionExpressionASTNode;
	
};

