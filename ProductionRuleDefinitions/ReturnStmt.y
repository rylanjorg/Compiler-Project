
ReturnStmt: RETURN NonAssignExpr SEMICOLON
{
	printf(ANSI_COLOR_BRIGHT_GREEN "Recognized Rule: RETURN\n" ANSI_COLOR_RESET);
    $$ = createNode(NULL, $2, NODE_RETURN); // Create a new AST node for RETURN
};
