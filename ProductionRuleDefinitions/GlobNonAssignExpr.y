
GlobNonAssignExpr:IDENTIFIER
{
	push("IDENTIFIER");
	struct ASTNode* identifier_node = GetASTNodeRef(head, $1,globScope, false, "");
	int update_eval = TryIncrementVarNumRef(head, $1,globScope);
	printf(ANSI_COLOR_BRIGHT_MAGENTA "\n IDENTIFIER %s pointer address: %p"ANSI_COLOR_RESET, $1,(void*)identifier_node);


	if(identifier_node != NULL)
	{
		// -------------------------------- Manage AST Node Generation --------------------------------
		//$$ = identifier_node;

		// Try and get the current value from the symbol table
		char* symtable_value = TryGetVarValue(head,$1, globScope);

		// If we got the value from the symbol table replace the identifier with its value
		if (symtable_value != NULL) 
		{
			// Replace the identifier with its value
			char actualValue[50];
			strcpy(actualValue, symtable_value);
			printf(ANSI_COLOR_GREEN "\n Constant Propogation: Setting '%s' to '%s' " ANSI_COLOR_RESET, $1, actualValue);
			
			int update_eval = TryDecrementVarNumRef(head, $1,globScope);


			$$ = createConstantNode(actualValue, NULL_TYPE);
		}
		else
		{
			$$ = identifier_node;
		}

		// -------------------------------- Manage MIPS Code Generation -------------------------------- 
	}
	else
    {
        printf(ANSI_COLOR_BRIGHT_RED"\n SEMANTIC ERROR: identifier '%s' not found\n"ANSI_COLOR_RESET, $1);
        exit(1);
    }
		
}
| IDENTIFIER OPERATOR GlobNonAssignExpr
{
	push("IDENTIFIER OPERATOR");
	struct ASTNode* identifier_node = GetASTNodeRef(head, $1,globScope, false, "");
	int update_eval = TryIncrementVarNumRef(head, $1,globScope);
	printf(ANSI_COLOR_BRIGHT_MAGENTA "\n IDENTIFIER %s pointer address: %p"ANSI_COLOR_RESET, $1,(void*)identifier_node);


	if(identifier_node != NULL)
	{
		// -------------------------------- Constant propagation Optimization --------------------------------
		
		// Try and get the current value from the symbol table
		char* symtable_value = TryGetVarValue(head, $1, globScope);

		// If we got the value from the symbol table replace the identifier with its value
		if (symtable_value != NULL) 
		{
			// Replace the identifier with its value
			char actualValue[50];
			strcpy(actualValue, symtable_value);
			printf(ANSI_COLOR_GREEN "\n Constant Propogation: Setting '%s' to '%s' " ANSI_COLOR_RESET, $1, actualValue);
			
			int update_eval = TryDecrementVarNumRef(head, $1,globScope);

			// -------------------------------- Constant folding Optimization --------------------------------

			if($3->node_type == NODE_CONSTANT)
			{
				char fc_str[50];	

				int folded_constant;

				if(strcmp($2, "+") == 0)
				{
					folded_constant = atoi(actualValue) + atoi($3->value);
				}
				else if(strcmp($2, "-") == 0)
				{
					folded_constant = atoi(actualValue) - atoi($3->value);
				}
				else if(strcmp($2, "*") == 0)
				{
					folded_constant = atoi(actualValue) * atoi($3->value);
				}
				else if(strcmp($2, "/") == 0)
				{
					folded_constant = atoi(actualValue) / atoi($3->value);
				}

				sprintf(fc_str, "%d", folded_constant);

				printf(ANSI_COLOR_GREEN "\n Constant folded: '%d' %s '%d' = '%d' " ANSI_COLOR_RESET, atoi(actualValue), $2, atoi($3->value), folded_constant);

			

				$$ = createConstantNode(fc_str, NULL_TYPE);
			}
			else
			{
				$$ = createBinaryExpressionNode($2, createConstantNode(actualValue, NULL_TYPE), $3);
			}
			// -------------------------------- -------------------------------- --------------------------------

		}
		else
		{
			$$ = createBinaryExpressionNode($2, identifier_node, $3);
		}

		// -------------------------------- Manage MIPS Code Generation -------------------------------- 

	}
	else
	{
		printf(ANSI_COLOR_BRIGHT_RED"\n SEMANTIC ERROR: identifier '%s' not found\n"ANSI_COLOR_RESET, $1);
		exit(1);
	}
}
| INT_LITERAL OPERATOR GlobNonAssignExpr
{

	if($3->node_type == NODE_CONSTANT)
	{
		char fc_str[50];	

		int folded_constant;

		if(strcmp($2, "+") == 0)
		{
			folded_constant = $1 + atoi($3->value);
		}
		else if(strcmp($2, "-") == 0)
		{
			folded_constant = $1 - atoi($3->value) ;
		}
		else if(strcmp($2, "*") == 0)
		{
			folded_constant = $1 * atoi($3->value);
		}
		else if(strcmp($2, "/") == 0)
		{
			folded_constant = $1 / atoi($3->value);
		}
		


		sprintf(fc_str, "%d", folded_constant);

		printf(ANSI_COLOR_GREEN "\n Constant folded: '%d' %s '%d' = '%d' " ANSI_COLOR_RESET, $1, $2, atoi($3->value), folded_constant);
		
		$$ = createConstantNode(fc_str, INT);
	}
	else
	{
		char c_str[50];
		sprintf(c_str, "%d", $1);
		$$ = createBinaryExpressionNode($2, createConstantNode(c_str, INT), $3);	
	}

} 
| INT_LITERAL                
{ 
	char str[50];
	sprintf(str, "%d", $1); 

	$$ = createConstantNode(str, INT);
	
};  