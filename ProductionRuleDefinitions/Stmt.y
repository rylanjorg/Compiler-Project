Stmt: SEMICOLON	
{
	printf(ANSI_COLOR_BRIGHT_GREEN " 	RECOGNIZED RULE: stmt -> SEMICOLON "ANSI_COLOR_RESET);
}
| Expr SEMICOLON	
{	
	printf(ANSI_COLOR_BRIGHT_GREEN " 	RECOGNIZED RULE: stmt -> Expr SEMICOLON \n"ANSI_COLOR_RESET);
	if($1 != NULL)
		$$ = createNode(NULL, $1, NODE_STMT);
	else
	{
		$$ = NULL;
	}
}
| IF LPAR ConditionalExpression RPAR ConditionalBlock
{
	printf(ANSI_COLOR_BRIGHT_GREEN " 	RECOGNIZED RULE: stmt -> IF LPAR RPAR Block \n"ANSI_COLOR_RESET);
	//$$ = createNode(NULL, $4, NODE_STMT);
}
| IF LPAR ConditionalExpression RPAR ConditionalBlock ELSE ConditionalBlock
{
	printf(ANSI_COLOR_BRIGHT_GREEN " 	RECOGNIZED RULE: stmt -> IF LPAR RPAR Block ELSE Block \n"ANSI_COLOR_RESET);
	//$$ = createNode($4, $6, NODE_STMT);
}
| WhileBlock
{ 
	printf(ANSI_COLOR_BRIGHT_GREEN " 	RECOGNIZED RULE: While Block \n"ANSI_COLOR_RESET);
}
| IfBlock
{ 
	printf(ANSI_COLOR_BRIGHT_GREEN " 	RECOGNIZED RULE: IF Block \n"ANSI_COLOR_RESET);
};
