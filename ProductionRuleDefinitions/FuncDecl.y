// summary:
// 1. Check whether the function exists in the glboal symbol table
// 2. If the function is not in the symbol table, add it to the symbol table and process the function scope.
// 3. AstNode declaration
// 4. Item kind declaration
// 5. Add the function to the global symbol table, then resolve the function scope for its instructions
// 6. Create a new symbol table for the function scope
// 7. Display the symbol table
// 8. Resolve the function scope

FuncDecl: TYPE IDENTIFIER LPAR ParamDeclList RPAR Block
{
	printf(ANSI_COLOR_BRIGHT_YELLOW "\n #####- Looking through symbol table for identifier '%s' -##### \n" ANSI_COLOR_RESET, $2);
	int inSymTab = CheckForIdentifierInSymbTab(&head, $2, globScope, false);

	if(inSymTab == 0)
	{
		printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: declaration for global function '%s'\n" ANSI_COLOR_RESET, $2);
		struct ASTNode* funcDecl = createFunctionDeclarationNode($4, $6, $2, $1);
		funcDecl->funcScope = $2;
		$$ = funcDecl;

		char itemKind[8];
		strcpy(itemKind, "Func");

		addItemToGlobalSymTab(head, $2, itemKind, $1, funcDecl); 
		insertSymbolTable(&head, $2, FunctionTable, numLocalConditionalBlocks);


		
		ResetConditionalBlockCounter();


		ShowAllSymTabs(head);

		
		if(strcmp($2, "main") == 0)
		{
			funcDecl->node_type = NODE_MAIN_DECL;
			ResolveFunctionScope($2, $$);
		}
		else
		{
			CopyCurrentInstructionStackToSymbolTable($2);
		}
	}
	else
    {
        printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Function %s is already in the symbol table\n"ANSI_COLOR_RESET, $2);
        exit(1);
    }

};
