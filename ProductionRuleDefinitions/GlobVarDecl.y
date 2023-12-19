GlobVarDecl: TYPE IDENTIFIER SEMICOLON	
{ 
	printf(ANSI_COLOR_BRIGHT_YELLOW "\n #####- Looking through symbol table for identifier '%s' -##### \n" ANSI_COLOR_RESET, $2);

	int inSymTab = CheckForIdentifierInSymbTab(head, $2,globScope, false);
								
	if (inSymTab == 0)
	{
		printf(ANSI_COLOR_BRIGHT_YELLOW "\n #####- identifier '%s' was not found in the global symbol table -##### \n" ANSI_COLOR_RESET, $2);
		printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: declaration for global variable '%s'\n" ANSI_COLOR_RESET, $2); 

		struct ASTNode* ver_decl = createVariableDeclarationNode($2, $1);
		ver_decl->isInitialized = false; // Initialize to false


		/// summary:
		/// Add the item to glob Symbol table since the scope is known at this time.
		char itemKind[8];
		strcpy(itemKind, "Var");
		
		addItemToGlobalSymTab(head, $2, itemKind, $1, ver_decl);
		$$ = ver_decl;
	}
	else
	{
		printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Var %s is already in the global symbol table\n"ANSI_COLOR_RESET, $2);
		exit(1);
	}

	ShowAllSymTabs(head);
	

}
| GlobalTypeExpr SEMICOLON
{
	$$ = createNode(NULL, $1, NODE_GLOB_VAR_BLOCKLIST);
	printStackContents();  
}
| TYPE IDENTIFIER LBRACKET INT_LITERAL RBRACKET SEMICOLON
{
	int num = $4;
	printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: declaration for global array of size : %d\n" ANSI_COLOR_RESET, num); 

	struct ASTNode* arrayDeclNode = createArrayDeclarationNode($2, $1, num);
	$$ = arrayDeclNode;
};


