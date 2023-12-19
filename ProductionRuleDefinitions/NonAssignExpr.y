
NonAssignExpr:IDENTIFIER
{
	// Schedule to resolve the identifier scope later. Will just need to search to see if it exists in the  current symbol table scope.

	char itemKind[8];
	strcpy(itemKind, "Ident");

	char itemType[8];
	strcpy(itemType, "NAsEx");

	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction($1, itemKind, itemType, NonAssignExpr_IDENTIFIER, 2);
	SetInstructionASTNodeRef(funcInstruction, NULL, 0);
	
	char tempValue[50];
	strcpy(tempValue, "null");

	
	struct ASTNode* constantNode = createConstantNode(tempValue, NULL_TYPE);
	struct ASTNode* assignmentNode = createNode(NULL, constantNode , NODE_ASSIGNMENT);

	SetInstructionASTNodeRef(funcInstruction, assignmentNode, 1);

	PushFuncInstruction(funcInstruction);
	$$ = assignmentNode;
}
| IDENTIFIER LPAR ParamList RPAR
{
	/// summary:
	/// So the ParamList is a list of ASTNodes that are references to the assignment nodes in the symbol table. This is processed in the ResolveFunctionScope function.
	/// So since the ParamList gets processed first, we have references to each of the params we are passing to the function.
	/// 
	/// Goal Outline:
	///
	/// - Use the ParamList to get the references to the param assignment nodes and assign each to the corresponding decl params in the function. [This handles passing the data to the function]
	///	     - SEMANTIC CHECKS: 
	///			- variable types need to match
	///			- the number of parameters need to match
	///	
	///	Im not sure how to handle the return, mainly just because idk how to handle optimizations. To outline the problem:
	///	
	///	- The return Node already holds the return value due to optimizations. If all of the code in the function can be marked as dead, 
	///   then the function doesn't need to made at all in the IRCode and MIPS code. But, if there are some dependencies, then the function needs to be made.
	///   The return node can still be optimized by constant propogation and constant folding, but the non dead code cant be removed. For example:
	///   	
	///	  FuncExample(int a, int b)
	///   {
	///		int k = a + b;
	///		int q = 5;
	///		write q;
	///		q = k;
	///		return k;
	///	  }



	printf(ANSI_COLOR_BRIGHT_GREEN "Recognized Rule: Function Expr\n" ANSI_COLOR_RESET);
	printAST($3,0,0);



	char itemKind[8];
	strcpy(itemKind, "");

	char itemType[8];
	strcpy(itemType, "Func");

	char tempValue[50];
	strcpy(tempValue, "Temp Null");

	int numParams = CountInstances($3, NODE_PARAM);

	struct ASTNode* returnNode = createFunctionCallNode(NULL,createConstantNode(tempValue, NULL_TYPE), $1, numParams);

	if(returnNode == NULL) 
	{ 
		printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: failed to create ASTNode for function call to '%s' \n"ANSI_COLOR_RESET, $1);
		exit(1);
	}
	else if(returnNode != NULL)
	{
		printf(ANSI_COLOR_BRIGHT_GREEN "\nRECOGNIZED RULE: Function call to '%s'. Successfully created ASTNode '%p' \n"ANSI_COLOR_RESET, $1, returnNode);
	}

	

	//struct ASTNode* constantNode = createConstantNode(actualValue);
	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction($1, itemKind, itemType, IDENTIFIER_LPAR_ParamList_RPAR, 2);
	SetInstructionASTNodeRef(funcInstruction, returnNode, 0);

	//struct Instruction* funcInstruction = CreateInstruction($1, itemKind, itemType, functionCallNode, IDENTIFIER_LPAR_ParamList_RPAR);
	
	SetInstructionASTNodeRef(funcInstruction, $3, 1);
	PushFuncInstruction(funcInstruction);
	
	$$ = returnNode;
	//$$ = $3;

}
| IDENTIFIER OPERATOR NonAssignExpr 
{

	struct ASTNode* returnNode = NULL;

	char itemKind[8];
	strcpy(itemKind, $2);

	char itemType[8];
	strcpy(itemType, "IdOp");

	char tempValue[50];
	strcpy(tempValue, "Temp Null");
	
	// Create a Binary expression node for the IDENTIFIER Operatore NonAssignExpr rule. 
	// If the NonAssignExpr evaluates to a constant and the IDENTIFIER in the "IDENTIFIER OPERATOR NonAssignExpr"
	// is not of type param, then just create a constant node and pass it to the binary expression node.
	// Since we can't evalueate whether an IDENTIFIER is a param until the IDENTIFIER is in the symboltable, we
	// have to wait until the function scope is known to process whether the constant propogation can occur. 
	// So to summarize:
	// - Create Binary expression node 
	// - then during the ResolveFunctionScope function, check whether the IDENTIFIER is a param. If it is, then 
	// dont constant fold, else constant fold.

	/*if($3->node_type == NODE_CONSTANT) 
	{ 
		returnNode = createConstantNode(tempValue, NULL_TYPE); 
	}
	else 
	{ */
	returnNode = createBinaryExpressionNode($2, createConstantNode(tempValue, NULL_TYPE), $3); 
	//}

	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction($1, itemKind, itemType, IDENTIFIER_OPERATOR_NonAssignExpr, 3);
	SetInstructionASTNodeRef(funcInstruction, returnNode, 0);

	// Instead of $3->value, pass the node created for $3 to the instruction
	SetInstructionASTNodeRef(funcInstruction, $3, 1);

	printf(ANSI_COLOR_BRIGHT_GREEN "\n SetInstructionChild(funcInstruction, returnNode) \n" ANSI_COLOR_RESET);
	PushFuncInstruction(funcInstruction);

	$$ = returnNode;

}
| INT_LITERAL OPERATOR NonAssignExpr 
{
	struct ASTNode* returnNode = NULL;


	char c_str[50];
	sprintf(c_str, "%d", $1);

	// Create a constant node for the left number
	struct ASTNode* constantNode = createConstantNode(c_str, INT);

	
	char itemKind[8];
	strcpy(itemKind, $2);

	char itemType[8];
	strcpy(itemType, "NumOp");

	char tempValue[50];
	strcpy(tempValue, "null");

	/*if($3->node_type == NODE_CONSTANT) 
	{ 
		returnNode = createConstantNode(tempValue, INT); 
	}
	else 
	{ 
		returnNode = createBinaryExpressionNode($2, createConstantNode(tempValue, INT), $3); 
	}*/

	returnNode = createBinaryExpressionNode($2, createConstantNode(tempValue, INT), $3); 

	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction(c_str, itemKind, itemType, NUMBER_OPERATOR_NonAssignExpr, 3);
	SetInstructionASTNodeRef(funcInstruction, returnNode, 0);

	// Instead of $3->value, pass the node created for $3 to the instruction
	SetInstructionASTNodeRef(funcInstruction, $3, 1);

	// Set the 2nd index to the constant node
	SetInstructionASTNodeRef(funcInstruction, constantNode, 2);

	printf(ANSI_COLOR_BRIGHT_GREEN "\n SetInstructionChild(funcInstruction, returnNode) \n" ANSI_COLOR_RESET);
	PushFuncInstruction(funcInstruction);

	$$ = returnNode;
} 
| FLOAT_LITERAL OPERATOR NonAssignExpr 
{
	struct ASTNode* returnNode = NULL;
	
	char c_str[50];
	sprintf(c_str, "%f", $1);

	// Create a constant node for the left number
	struct ASTNode* constantNode = createConstantNode(c_str, FLOAT);



	char itemKind[8];
	strcpy(itemKind, $2);

	char itemType[8];
	strcpy(itemType, "NumOp");


	char tempValue[50];
	strcpy(tempValue, "null");

	/*
	if($3->node_type == NODE_CONSTANT) 
	{ 
		returnNode = createConstantNode(tempValue, FLOAT); 
	}
	else 
	{ 
		returnNode = createBinaryExpressionNode($2, createConstantNode(tempValue, FLOAT), $3); 
	}*/

	returnNode = createBinaryExpressionNode($2, createConstantNode(tempValue, FLOAT), $3); 

	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction(c_str, itemKind, itemType, NUMBER_OPERATOR_NonAssignExpr, 3);
	SetInstructionASTNodeRef(funcInstruction, returnNode, 0);

	// Instead of $3->value, pass the node created for $3 to the instruction
	SetInstructionASTNodeRef(funcInstruction, $3, 1);

	// Set the 2nd index to the constant node
	SetInstructionASTNodeRef(funcInstruction, constantNode, 2);



	printf(ANSI_COLOR_BRIGHT_GREEN "\n SetInstructionChild(funcInstruction, returnNode) \n" ANSI_COLOR_RESET);
	PushFuncInstruction(funcInstruction);

	$$ = returnNode;
} 
| IDENTIFIER LBRACKET INT_LITERAL RBRACKET OPERATOR NonAssignExpr
{
	struct ASTNode* returnNode = NULL;
	
	char arrayIdentifier[50];
	strcpy(arrayIdentifier,$1);


	



	char itemKind[8];
	strcpy(itemKind, $5);

	char itemType[8];
	sprintf(itemType, "%d", $3);

	char tempValue[50];
	strcpy(tempValue, "null");

	// Create a constant node for the left number
	struct ASTNode* constantNode = createConstantNode(tempValue, ARRAY_TEMP_ELEMENT);

	returnNode = createBinaryExpressionNode($5, createConstantNode(tempValue, ARRAY_TEMP_ELEMENT), $6); 

	// Create the instruction to be processed once the function scope is known.
	struct Instruction* funcInstruction = CreateInstruction(arrayIdentifier, itemKind, itemType, NUMBER_OPERATOR_NonAssignExpr, 3);
	SetInstructionASTNodeRef(funcInstruction, returnNode, 0);

	// Instead of $6->value, pass the node created for $6 to the instruction
	SetInstructionASTNodeRef(funcInstruction, $6, 1);

	// Set the 2nd index to the constant node
	SetInstructionASTNodeRef(funcInstruction, constantNode, 2);

	PushFuncInstruction(funcInstruction);

	$$ = returnNode;
}
| FLOAT_LITERAL              
{ 
	char str[50];
	sprintf(str, "%f", $1); 

	printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: FLOAT_LITERAL -> '%s'\n" ANSI_COLOR_RESET, str);
	
	$$ = createConstantNode(str, FLOAT);
};  
| INT_LITERAL                
{ 
	char str[50];
	sprintf(str, "%d", $1); 

	printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: INT_LITERAL -> '%s'\n" ANSI_COLOR_RESET, str);

	$$ = createConstantNode(str, INT);
}
| CHAR_LITERAL
{
	char charValue[50];

	// Copy the character to the array and null terminate the string.
	charValue[0] = $1;
	charValue[1] = '\0';

	printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: CHAR_LITERAL -> %s \n" ANSI_COLOR_RESET, charValue);

	$$ = createConstantNode(charValue, CHAR);
}
| IDENTIFIER LBRACKET INT_LITERAL RBRACKET
{
	char tempValue[50];
	strcpy(tempValue, "Temp Null [IDENTIFIER LBRACKET INT_LITERAL RBRACKET]");

	printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: IDENTIFIER LBRACKET INT_LITERAL RBRACKET \n" ANSI_COLOR_RESET);

	$$ = createConstantNode(tempValue, ARRAY_TEMP_ELEMENT);
};





