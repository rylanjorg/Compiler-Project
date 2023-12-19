
GlobalTypeExpr: TYPE IDENTIFIER {}
| TYPE IDENTIFIER EQ GlobNonAssignExpr   
{ 
	printf(ANSI_COLOR_BRIGHT_YELLOW "\n #####- Looking through symbol table for identifier '%s' -##### \n" ANSI_COLOR_RESET, $2);

    int inSymTab = CheckForIdentifierInSymbTab(head, $2,globScope, false);

    if (inSymTab == 0)
    {
        printf(ANSI_COLOR_BRIGHT_YELLOW "\n #####- identifier '%s' was not found in the global symbol table -##### \n" ANSI_COLOR_RESET, $2);
        printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: declaration for global variable '%s'\n" ANSI_COLOR_RESET, $2); 

        struct ASTNode* ver_decl = createVariableDeclarationNode($2, $1);

        /// summary:
        /// Add the item to glob Symbol table since the scope is known at this time.
        char itemKind[8];
        strcpy(itemKind, "Var");

        // Mark the variable as initialized
        int update_eval = TryIncrementVarNumRef(head,$2,globScope);
        ver_decl->isInitialized = true;

		addItemToGlobalSymTab(head, $2, itemKind, $1, ver_decl);

		if($4->node_type == NODE_CONSTANT)
		{
			printf(ANSI_COLOR_YELLOW "\n%s $4->node_type == NODE_CONSTANT, $4->value == '%s'\n" ANSI_COLOR_RESET,"           ", $4->value); 

			char str[50];	
			strcpy(str, $4->value);

			struct ASTNode* assignmentNode = createNode(ver_decl, $4, NODE_ASSIGNMENT);

			printf(ANSI_COLOR_GREY "%s--> Created Assignment Node with pointer '%p', right value '%s', and left varDeclNode (value: '%s' type: '%s') \n"ANSI_COLOR_RESET, "           ", (void*)assignmentNode, $4->value, ver_decl->value, ver_decl->type);
        	int updated_assin_ref = TryUpdateAssignmentNodeRef(head, $2, globScope, assignmentNode);
			if(updated_assin_ref == 0) 	
			{
				printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Could not update Assignment node ref because it could not be found in the symbol table with scope '%s'\n"ANSI_COLOR_RESET, globScope);
				exit(1);
			}
			int updated_var = TryUpdateVarValue(head, $2, globScope, str);

			$$ = assignmentNode;
		}
		else
		{
			printf(ANSI_COLOR_BRIGHT_RED "\nWARNING: $4->node_type != NODE_CONSTANT" ANSI_COLOR_RESET);
			$$ = ver_decl;
		}
    }
    else
    {
        printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Var %s is already in the global symbol table\n"ANSI_COLOR_RESET, $2);
        exit(1);
    }

    ShowAllSymTabs(head);
	
}
| TYPE IDENTIFIER LBRACKET INT_LITERAL RBRACKET EQ GlobNonAssignExpr    
{
	push("IDENTIFIER LBRACKET NUMBER RBRACKET EQ");
	printStackContents(); 
};


