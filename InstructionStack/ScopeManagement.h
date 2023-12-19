#ifndef PRINT_INTERMEDIATE_AST
#define PRINT_INTERMEDIATE_AST 1
#endif

#ifndef SCOPE_MANAGEMENT_H
#define SCOPE_MANAGEMENT_H



char indentation[50] = "           ";
    
// -------------------------------- START STACK --------------------------------


#include <string.h>
#include <stdio.h>

#include "ConstantOptimizations.h"
#include "WhileLoopSemanticChecks.h"


#include "../Helpers/ConsoleFormating.h"
#include "../SymbolTableManagement/SymbolTableManager.h"
#include "../AST/AST.h"
#include "../IRCode/IRcodeMain.h"
#include "../Helpers/Dictionary.h"


#define MAX_FUNC_INSTRUCTIONS 100

// Define the first stack
struct Instruction* funcInstructionStack[MAX_FUNC_INSTRUCTIONS];  
int topFuncInstruction = -1;





struct Instruction* CreateInstruction(char itemName[50], char itemKind[8], char itemType[8], InstructionType instructionType, int size)
{
    //Create an instance of the instruction struct
    struct Instruction* instruction = (struct Instruction*)malloc(sizeof(struct Instruction));

    // Allocate memory for the array node
    struct ArrayDeclarationNode* arrayNode = malloc(sizeof(struct ArrayDeclarationNode));
    arrayNode->size = size;

    // Allocate memory for the elements array
    arrayNode->elements = malloc(size * sizeof(struct ASTNode*));

    // Initialize elements to NULL (or to some default value if needed)
    for (int i = 0; i < size; i++) 
    {
        arrayNode->elements[i] = NULL;
    }

    instruction->arrayNode = arrayNode;


    strcpy(instruction->itemName, itemName);
    strcpy(instruction->itemKind, itemKind);
    strcpy(instruction->itemType, itemType);
    instruction->instructionType = instructionType;
    return instruction;
}

struct Instruction* CreateBreakInstruction()
{
    struct Instruction* instruction = (struct Instruction*)malloc(sizeof(struct Instruction));
    int size = 1;

    // Allocate memory for the array node
    struct ArrayDeclarationNode* arrayNode = malloc(sizeof(struct ArrayDeclarationNode));
    arrayNode->size = size;

    // Allocate memory for the elements array
    arrayNode->elements = malloc(size * sizeof(struct ASTNode*));

    // Initialize elements to NULL (or to some default value if needed)
    for (int i = 0; i < size; i++) 
    {
        arrayNode->elements[i] = NULL;
    }

    instruction->arrayNode = arrayNode;



    strcpy(instruction->itemName, "");
    strcpy(instruction->itemKind, "");
    strcpy(instruction->itemType, "");
    instruction->instructionType = INSTRUCTION_BREAK;
    return instruction;
}

void SetInstructionASTNodeRef(struct Instruction* instruction, struct ASTNode* child_ASTNodeRef, int index)
{
    if(index >= 0 && index <= instruction->arrayNode->size-1) 
    { 
       instruction->arrayNode->elements[index] = child_ASTNodeRef;
    }
    else
    {
        printf(ANSI_COLOR_BRIGHT_RED "\nERROR: Index '%d' is out of bounds for instruction array [name: '%s' kind: '%s' type: '%s' instructionType: '%d'] of size '%d'. \n" ANSI_COLOR_RESET, 
        index, instruction->itemName, instruction->itemKind, instruction->itemType, instruction->instructionType, instruction->arrayNode->size); exit(1); 
    }

    
}


void SetAssignmentNodeRef(struct Instruction* instruction, struct ASTNode* assignmentNodeRef)
{
    instruction->assignmentNodeRef = assignmentNodeRef;
}



// Push an element onto the second stack
void PushFuncInstruction(struct Instruction* element) 
{
    if (topFuncInstruction < MAX_FUNC_INSTRUCTIONS - 1) 
    {
        topFuncInstruction ++;
        funcInstructionStack[topFuncInstruction] = element;
    } else 
    {
        printf("Second stack overflow\n");
    }
}

// Pop an element from the second stack
void PopFuncInstruction() 
{
    if (topFuncInstruction >= 0) 
    {
        topFuncInstruction--;
    } else 
    {
        printf("Second stack underflow\n");
    }
}

// Get the top element from the second stack
struct Instruction* GetInstructionFromStack() 
{
    if (topFuncInstruction >= 0) 
    {
        return funcInstructionStack[topFuncInstruction];
    } else 
    {
        return NULL;
    }
}

// Clear the second stack
void ClearFuncInstruction() 
{
    topFuncInstruction = -1;
}

void ReverseFuncInstructionStack() 
{
    if (topFuncInstruction < 0) 
    {
        printf("Stack is empty. Cannot reverse.\n");
        return;
    }

    struct Instruction* tempStack[MAX_FUNC_INSTRUCTIONS];
    int tempTop = -1;

    // Pop elements from original stack and push them onto temporary stack
    while (topFuncInstruction >= 0) 
    {
        tempTop++;
        tempStack[tempTop] = funcInstructionStack[topFuncInstruction];
        topFuncInstruction--;
    }

    // Copy elements from temporary stack back into original stack
    for (int i = 0; i <= tempTop; i++) 
    {
        topFuncInstruction++;
        funcInstructionStack[topFuncInstruction] = tempStack[i];
    }
}

void DisplayFuncInstructionStack() 
{
    if (topFuncInstruction < 0) 
    {
        printf("Stack is empty. Nothing to display.\n");
        return;
    }

    printf("%sCurrent stack order:\n", indentation);

    for (int i = topFuncInstruction; i >= 0; i--) 
    {
        printf("%s%d.) name: %s kind: %s type: %s instructionType: %d \n", indentation,topFuncInstruction - i + 1, funcInstructionStack[i]->itemName, funcInstructionStack[i]->itemKind, funcInstructionStack[i]->itemType, funcInstructionStack[i]->instructionType);
    }
}



// -------------------------------- END STACK  --------------------------------

bool localVarDecl(struct Instruction* varDeclInstruction, char scope[50])
{

    printf(ANSI_COLOR_BRIGHT_YELLOW "           #####- Looking through symbol table for identifier '%s' -##### \n" ANSI_COLOR_RESET, varDeclInstruction->itemName);

    // Check whether the IDENTIFIER in the "TYPE IDENTIFIER EQ NonAssignExpr" is in the symbol table for the function scope. 
    // If it does not exist, do not check the global scope since this is a local variable declaration.
	int inSymTab = CheckForIdentifierInSymbTabIndent(head, varDeclInstruction->itemName, scope, indentation, false);

	if (inSymTab == 0)
	{
		printf(ANSI_COLOR_GREY "%s--> Info: IDENTIFIER '%s' was not found to be previously declared in the function scope '%s' \n" ANSI_COLOR_RESET, indentation, varDeclInstruction->itemName, scope);
        printf(ANSI_COLOR_BRIGHT_GREEN "RECOGNIZED RULE: declaration for local variable '%s' initalization with assignment '%s = %s' in scope '%s' \n" ANSI_COLOR_RESET, 
        varDeclInstruction->itemName, varDeclInstruction->itemName, varDeclInstruction->assignmentNodeRef->right != NULL ? varDeclInstruction->assignmentNodeRef->right->value : "(NULL)", scope); 
        
        /// summary: 
        /// Add IDENTIFIER to the function symbol table. The declaration of the function for that is as follows:
        /// addItemToFuncSymTab(struct SymbolTableNode* funcSymTabNode, char itemName[50], char itemKind[8], char itemType[8], struct ASTNode* ast_node_ref)
        addItemToFuncSymTab(head, scope, varDeclInstruction->itemName, varDeclInstruction->itemKind, varDeclInstruction->itemType,varDeclInstruction->arrayNode->elements[0]);
	}
	else
	{
		printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Var %s is already in the symbol table\n" ANSI_COLOR_RESET, varDeclInstruction->itemName);
		return false;
	}

    /// summary:
    /// This is just a copy of the Resolve_TYPE_IDENTIFIER_EQ_NonAssignExpr,expect I dont set the assignment value to anything
   

    printf(ANSI_COLOR_GREY"\n%s--> Info: As the right pointer of the Assignment Node should point to Null (actual pointer value is -> '%p')." ANSI_COLOR_RESET, 
    indentation, varDeclInstruction->assignmentNodeRef->right != NULL ? varDeclInstruction->assignmentNodeRef->right : "(NULL)");

    // Update the assignment node reference in the symbol table
    int updated_assin_ref = TryUpdateAssignmentNodeRef(head, varDeclInstruction->itemName, scope, varDeclInstruction->assignmentNodeRef);

    // Incremenet the number of references to the variable
	int update_eval = TryIncrementVarNumRef(head,varDeclInstruction->itemName,scope);

    return true;
}


bool ParamDecl(struct Instruction* paramDeclInstruction, char scope[50])
{
    printf(ANSI_COLOR_BRIGHT_YELLOW "           #####- Looking through symbol table for identifier '%s' -##### \n" ANSI_COLOR_RESET, paramDeclInstruction->itemName);

    char indentation[50];
    strcpy(indentation, "           ");

	int inSymTab = CheckForIdentifierInSymbTabIndent(head, paramDeclInstruction->itemName, scope, indentation, false);

	if (inSymTab == 0)
	{
		printf(ANSI_COLOR_BRIGHT_GREEN "\n%sRECOGNIZED RULE: declaration for local variable '%s'" ANSI_COLOR_RESET, indentation, paramDeclInstruction->itemName); 
        // void addItemToFuncSymTab(struct SymbolTableNode* funcSymTabNode, char itemName[50], char itemKind[8], char itemType[8], struct ASTNode* ast_node_ref)
        addItemToFuncSymTab(head, scope, paramDeclInstruction->itemName, paramDeclInstruction->itemKind, paramDeclInstruction->itemType, paramDeclInstruction->arrayNode->elements[0]);

        // Display the symbol table to show the update
        struct SymbolTableNode* stNode = GetSymbolTableFromLLScope(head, scope);
        showFuncSymTable(stNode, false);
        return true;
	}
	else
	{
		printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Var %s is already in the symbol table\n" ANSI_COLOR_RESET, paramDeclInstruction->itemName);
		return false;
	}
}

// Done
bool Resolve_TYPE_IDENTIFIER_EQ_NonAssignExpr(struct Instruction* varDeclInstruction, char scope[50])
{
    

    printf(ANSI_COLOR_BRIGHT_YELLOW "           #####- Looking through symbol table for identifier '%s' -##### \n" ANSI_COLOR_RESET, varDeclInstruction->itemName);

    // Check whether the IDENTIFIER in the "TYPE IDENTIFIER EQ NonAssignExpr" is in the symbol table for the function scope. 
    // If it does not exist, do not check the global scope since this is a local variable declaration.
	int inSymTab = CheckForIdentifierInSymbTabIndent(head, varDeclInstruction->itemName, scope, indentation, false);

	if (inSymTab == 0)
	{
		printf(ANSI_COLOR_GREY "%s--> Info: IDENTIFIER '%s' was not found to be previously declared in the function scope '%s' \n" ANSI_COLOR_RESET, indentation, varDeclInstruction->itemName, scope);
        //printf(ANSI_COLOR_BRIGHT_GREEN "RECOGNIZED RULE: declaration for local variable '%s' initalization with assignment '%s = %s' in scope '%s' \n" ANSI_COLOR_RESET, 
        //varDeclInstruction->itemName, varDeclInstruction->itemName, "(NULL)", scope); 
        // varDeclInstruction->assignmentNodeRef->right != NULL ? varDeclInstruction->assignmentNodeRef->right->value : 

        /// summary: 
        /// Add IDENTIFIER to the function symbol table. The declaration of the function for that is as follows:
        /// addItemToFuncSymTab(struct SymbolTableNode* funcSymTabNode, char itemName[50], char itemKind[8], char itemType[8], struct ASTNode* ast_node_ref)
        addItemToFuncSymTab(head, scope, varDeclInstruction->itemName, varDeclInstruction->itemKind, varDeclInstruction->itemType, varDeclInstruction->arrayNode->elements[0]);

	}
	else
	{
		printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Var %s is already in the symbol table\n" ANSI_COLOR_RESET, varDeclInstruction->itemName);
		return false;
	}


    /**
    printf(ANSI_COLOR_GREY "%s--> Info: IDENTIFIER '%s' was not found to be previously declared in the function scope '%s' \n" ANSI_COLOR_RESET, indentation, varDeclInstruction->itemName, scope);
    printf(ANSI_COLOR_BRIGHT_RED "\n Unrecognized NonAssignExpr.  \n" ANSI_COLOR_RESET);
    varDeclInstruction->assignmentNodeRef->right != NULL ? printf(" Assignment node ->right reference is not NULL") : printf(" Assignment node ->right reference is NULL");

    switch(varDeclInstruction->assignmentNodeRef->right->node_type)
    {
    case NODE_CONSTANT:
        printf(ANSI_COLOR_BRIGHT_GREEN "RECOGNIZED RULE: declaration for local variable '%s' initalization with assignment '%s = %s' in scope '%s' \n" ANSI_COLOR_RESET, 
        varDeclInstruction->itemName, varDeclInstruction->itemName, varDeclInstruction->assignmentNodeRef->right != NULL ? varDeclInstruction->assignmentNodeRef->right->value : "(NULL)", scope); 
        break;
    case NODE_FUNCTION_CALL:
        printf(ANSI_COLOR_BRIGHT_GREEN "RECOGNIZED RULE: declaration for local variable '%s' initalization to function call in scope '%s' \n" ANSI_COLOR_RESET, 
        varDeclInstruction->itemName, scope); 
        break;
    default:
        printf(ANSI_COLOR_BRIGHT_RED "\n Unrecognized NonAssignExpr. FunctionCall: Resolve_TYPE_IDENTIFIER_EQ_NonAssignExpr()"ANSI_COLOR_RESET);
        break;
    }

    printf(ANSI_COLOR_BRIGHT_RED "\n Unrecognized NonAssignExpr. FunctionCall: Resolve_TYPE_IDENTIFIER_EQ_NonAssignExpr() \n" ANSI_COLOR_RESET);

    /// summary: 
    /// Add IDENTIFIER to the function symbol table. The declaration of the function for that is as follows:
    /// addItemToFuncSymTab(struct SymbolTableNode* funcSymTabNode, char itemName[50], char itemKind[8], char itemType[8], struct ASTNode* ast_node_ref)
    addItemToFuncSymTab(head, scope, varDeclInstruction->itemName, varDeclInstruction->itemKind, varDeclInstruction->itemType,varDeclInstruction->ast_node_ref);
    */

    /// summary:
    /// So the parser handles the assignment node creation as follows:
    ///     
    ///     struct ASTNode* varDeclNode = createVariableDeclarationNode($2, $1);
    ///     struct Instruction* funcInstruction = CreateInstruction($2, itemKind, $1, varDeclNode, TYPE_IDENTIFIER_EQ_NonAssignExpr);
    ///
    ///     SetInstructionChild(funcInstruction, $4);
    ///     struct ASTNode* assignment = createNode(varDeclNode, NULL, NODE_ASSIGNMENT);
    ///     SetAssignmentNodeRef(funcInstruction, assignment);
    ///     $$ = assignment;
    ///
    /// So the assignment node is created during parsing, and is added to the AST tree. The right side of the 
    /// assignment node is set to NULL because it needs to be processed once the scope is known. So, I pass the 
    /// NonAssignExpr as the child of the instruction statement.
    ///
    /// So what needs to happen here now that the scope is known:
    /// 1. if it passes semantic checks -> Add the IDENTIFIER to the symbol table using the variable 
    ///                                    ASTNode reference passed in the instruction created during parsing.
    /// 2. if it passes semantic checks -> Process the NonAssignExpr ($4), and then assign it to the assignment 
    ///                                    node passed in as the assignment node reference in the instruction.
    /// 3. if it passes semantic checks -> Update the symbol table to point the assignment node reference to the
    ///                                    assignment node passed in that is now done being processed.


    //printf(ANSI_COLOR_GREY"\n%s--> Info: Need to get the value from the NonAssignExpr stored in the child ASTNode reference pointer -> '%p'." ANSI_COLOR_RESET, indentation, varDeclInstruction->child_ast_node_ref);
    
    



    if(varDeclInstruction->arrayNode->elements[1] == NULL) { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: Var %s was not updated because the child ASTNode reference is NULL\n" ANSI_COLOR_RESET, varDeclInstruction->itemName); return false; }

    char valueStr[50];	
    DataType leftType = CastStringToDataType(varDeclInstruction->itemType);
    DataType rightType = varDeclInstruction->arrayNode->elements[1]->nodeData.dataType;
    

    switch(varDeclInstruction->arrayNode->elements[1]->node_type)
    {
        case NODE_CONSTANT:
            strcpy(valueStr, varDeclInstruction->arrayNode->elements[1]->value);
            printf(ANSI_COLOR_BRIGHT_GREEN "\n%sRECOGNIZED RULE: declaration for local variable '%s' initalization with assignment NODE_CONSTANT '%s = %s' in scope '%s' " ANSI_COLOR_RESET, 
            indentation, varDeclInstruction->itemName, varDeclInstruction->itemName, varDeclInstruction->arrayNode->elements[2]->right != NULL ? varDeclInstruction->arrayNode->elements[2]->right->value : "(NULL)", scope); 
            varDeclInstruction->arrayNode->elements[2]->right = varDeclInstruction->arrayNode->elements[1];
            varDeclInstruction->arrayNode->elements[1]->nodeData.dataType = leftType;
            printf(ANSI_COLOR_GREY "\n%s--> Info: IDENTIFIER '%s' initalized to Data type '%d' " ANSI_COLOR_RESET, indentation, varDeclInstruction->itemName, leftType);
            break;
        case NODE_FUNCTION_CALL:
            strcpy(valueStr, varDeclInstruction->arrayNode->elements[1]->right->value);
            printf(ANSI_COLOR_BRIGHT_GREEN "\n%sRECOGNIZED RULE: declaration for local variable '%s' initalization to NODE_FUNCTION_CALL  '%s = %s' in scope '%s'. \n" ANSI_COLOR_RESET, 
            indentation, varDeclInstruction->itemName, varDeclInstruction->itemName, valueStr, scope); 
            varDeclInstruction->arrayNode->elements[2]->right = varDeclInstruction->arrayNode->elements[1]->right;
            varDeclInstruction->arrayNode->elements[1]->right->nodeData.dataType = leftType;
            printf(ANSI_COLOR_GREY "\n%s--> Info: IDENTIFIER '%s' initalized to Data type '%d' " ANSI_COLOR_RESET, indentation, varDeclInstruction->itemName, leftType);
            break;
        case NODE_BINARY_EXPRESSION:
            // result of binary expression is stored in the valuenode of the binary expression node
            strcpy(valueStr, varDeclInstruction->arrayNode->elements[1]->value);
            printf(ANSI_COLOR_BRIGHT_GREEN "\n%sRECOGNIZED RULE: declaration for local variable '%s' initalization to BINARY_EXPRESSION '%s = %s' in scope '%s'. \n" ANSI_COLOR_RESET, 
            indentation, varDeclInstruction->itemName, varDeclInstruction->itemName, valueStr, scope); 
            varDeclInstruction->arrayNode->elements[2]->right = varDeclInstruction->arrayNode->elements[1];
            varDeclInstruction->arrayNode->elements[1]->right->nodeData.dataType = leftType;
            printf(ANSI_COLOR_GREY "\n%s--> Info: IDENTIFIER '%s' initalized to Data type '%d' " ANSI_COLOR_RESET, indentation, varDeclInstruction->itemName, leftType);
            break;
        default:
            printf(ANSI_COLOR_BRIGHT_RED "\n Unrecognized NonAssignExpr. FunctionCall: Resolve_TYPE_IDENTIFIER_EQ_NonAssignExpr()"ANSI_COLOR_RESET);
            break;
    }
    

    // Now that we have both data types we can create the propogation identifiers
    struct PropogationIdentifier* left = initPropogationIdentifier("", leftType);
    struct PropogationIdentifier* right = initPropogationIdentifier(valueStr, rightType);

    // Type cast right to left
    TypeCast(right, left);

    // Print message about uodated data 
    printf(ANSI_COLOR_GREY"\n%s--> Info: The value of the NonAssignExpr was casted from the data type '%d' to the data type '%d' its value is '%s' " ANSI_COLOR_RESET, indentation, leftType, rightType, right->value);

    //char str[50];	
    //strcpy(str, varDeclInstruction->child_ast_node_ref->value);

    /*
    if(varDeclInstruction->child_ast_node_ref == NODE_CONSTANT)
    {
        printf(ANSI_COLOR_GREY"\n%s--> Info: The value of that NonAssignExpr is '%s'\n" ANSI_COLOR_RESET, indentation, varDeclInstruction->child_ast_node_ref->value != NULL ? varDeclInstruction->child_ast_node_ref->value : "(NULL)");
        varDeclInstruction->assignmentNodeRef->right = varDeclInstruction->child_ast_node_ref;
    } 
    else if(varDeclInstruction->child_ast_node_ref == NODE_FUNCTION_CALL)
    {
        printf(ANSI_COLOR_GREY"\n%s--> Info: The NonAssignExpr is a function call so getting the value from the right node \n" ANSI_COLOR_RESET, indentation);
        printf(ANSI_COLOR_GREY"\n%s--> Info: The value of that NonAssignExpr is '%s'\n" ANSI_COLOR_RESET, indentation, varDeclInstruction->child_ast_node_ref->right->value != NULL ? varDeclInstruction->child_ast_node_ref->right->value : "(NULL)");
        varDeclInstruction->assignmentNodeRef->right = varDeclInstruction->child_ast_node_ref->right;
    } 
    else
    */
    
    // Update the assignment node reference in the symbol table
    int updated_assin_ref = TryUpdateAssignmentNodeRef(head, varDeclInstruction->itemName, scope, varDeclInstruction->arrayNode->elements[2]);
    if(updated_assin_ref == 0) { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: failed to update the assignmnet node reference in the symbol table '%s' for IDENTIFIER '%s' \n" ANSI_COLOR_RESET, scope, varDeclInstruction->itemName); return false; }
    else { printf(ANSI_COLOR_GREY "\n%s--> Info: IDENTIFIER '%s' assignment node reference was updated in the symbol table of scope '%s' to pointer '%p' " ANSI_COLOR_RESET, indentation, varDeclInstruction->itemName, scope, varDeclInstruction->assignmentNodeRef); }
    
    // Incremenet the number of references to the variable
	int update_eval = TryIncrementVarNumRef(head,varDeclInstruction->itemName,scope);

    // Update the value in the symbol table to that of the assignment node
    int updated_var = TryUpdateVarValue(head, varDeclInstruction->itemName, scope, right->value);
    if(updated_var == 0) { printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Var %s was not updated because is not in the symbol table\n" ANSI_COLOR_RESET, varDeclInstruction->itemName); return false; }
    else { printf(ANSI_COLOR_GREY "\n%s--> Info: IDENTIFIER '%s' value was updated in the symbol table of scope '%s' to value '%s' " ANSI_COLOR_RESET, indentation, varDeclInstruction->itemName, scope, right->value); }

    printf("\n");
    ShowAllSymTabs(head);

    return true;
}

// Done
bool Resolve_IDENTIFIER_EQ(struct Instruction* varDeclInstruction, char scope[50], struct ASTNode* functionRootNode)
{
    printf(ANSI_COLOR_BRIGHT_YELLOW "           #####- Looking through symbol table for identifier '%s', target scope '%s' -##### \n" ANSI_COLOR_RESET, varDeclInstruction->itemName, scope);

    // copy the scope in case it gets changed by the checkforidentifier function
    char tempScope[50];
    strcpy(tempScope, scope);

    // Check whether the IDENTIFIER in the "IDENTIFIER EQ" is in the symbol table for the scope.
    // This is a SEMANTIC CHECK for whether the IDENTIFIER has been declared before it is used.
    
	int inSymTab = CheckForIdentifierInSymbTabIndent(head, varDeclInstruction->itemName, scope, indentation, true);

	if (inSymTab == 1)
	{
        printf(ANSI_COLOR_GREY "%s--> Info: IDENTIFIER '%s' was found to be previously declared in the function scope '%s' " ANSI_COLOR_RESET, indentation, varDeclInstruction->itemName, scope);

        // Get the IDENTIFIER Var ASTNode reference, and whether it has an assignment node reference
        struct ASTNode* identifier_node = GetASTNodeRef(head,  varDeclInstruction->itemName, scope, true,"");
        int has_assignment_node = DoesVarAlreadyHaveAssignment(head, varDeclInstruction->itemName, scope);

        // print a message to the console whether the IDENTIFIER has an assignment node reference
        if(has_assignment_node == 1) { printf(ANSI_COLOR_GREY "\n%s--> Info: IDENTIFIER '%s' has an existing assignment node in the symbol table in '%s' " ANSI_COLOR_RESET, indentation, varDeclInstruction->itemName, scope); }
        else { printf(ANSI_COLOR_GREY "\n%s--> Info: IDENTIFIER '%s' does not have an existing assignment node in the symbol table in '%s' " ANSI_COLOR_RESET, indentation, varDeclInstruction->itemName, scope); }


   
        // If the IDENTIFIER Var ASTNode has no assignment node reference, assign one, otherwise just update the assignment node value
        if(has_assignment_node == 0)
        { 
            printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE: Var '%s' has no assignment node reference, making an assignment node for Var '%s' \n" ANSI_COLOR_RESET, varDeclInstruction->itemName, varDeclInstruction->itemName); 

            // Assign the assignment not previously created while parsing. 
            int updated_assin_ref = TryUpdateAssignmentNodeRef(head, varDeclInstruction->itemName, scope, varDeclInstruction->arrayNode->elements[0]);
            if(updated_assin_ref == 0) { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: Var %s was not assigned an new assignment node reference of '%p' \n" ANSI_COLOR_RESET, varDeclInstruction->itemName, varDeclInstruction->arrayNode->elements[0]); return false;}

            // Update the value in the symbol table
            char str[50];
            strcpy(str, varDeclInstruction->arrayNode->elements[1]->value);
            int updated_var = TryUpdateVarValue(head, varDeclInstruction->itemName,scope, str);

            // Dead code handling
            int update_eval = TryIncrementVarNumRef(head, varDeclInstruction->itemName, scope);
            if(update_eval == 1){ printf(ANSI_COLOR_ORANGE "\n IDENTIFIER REFERENCE: IDENTIFIER '%s' references increased by 1\n" ANSI_COLOR_RESET, varDeclInstruction->itemName); }
            else { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: Var %s was not updated with new number of references\n" ANSI_COLOR_RESET, varDeclInstruction->itemName); return false; }
        }
        else
        {
            if(varDeclInstruction->arrayNode->elements[1] == NULL) {printf(ANSI_COLOR_BRIGHT_RED "\nERROR: IDENTIFIER %s was not updated because the child ASTNode reference is NULL\n" ANSI_COLOR_RESET, varDeclInstruction->itemName); return false; }
            
            


            char str[50];
            int updated_var;

            switch(varDeclInstruction->arrayNode->elements[1]->node_type)
            {
                case NODE_CONSTANT:
                    printf(ANSI_COLOR_BRIGHT_GREEN "\n%sRECOGNIZED RULE: reassignment for local IDENTIFIER '%s = %s' in scope '%s' \n" ANSI_COLOR_RESET, indentation, varDeclInstruction->itemName, varDeclInstruction->arrayNode->elements[1]->value, scope);
                    strcpy(str, varDeclInstruction->arrayNode->elements[1]->value);
                    updated_var = TryUpdateVarValue(head, varDeclInstruction->itemName,scope, str);
                    break;
                case NODE_FUNCTION_CALL:
                    printf(ANSI_COLOR_BRIGHT_GREEN "\n%sRECOGNIZED RULE: reassignment for local IDENTIFIER '%s = %s' in scope '%s' \n" ANSI_COLOR_RESET, indentation, varDeclInstruction->itemName, varDeclInstruction->arrayNode->elements[1]->right->value, scope);
                    strcpy(str, varDeclInstruction->arrayNode->elements[1]->right->value);
                    updated_var = TryUpdateVarValue(head, varDeclInstruction->itemName,scope, str); 
                    break;
                case NODE_BINARY_EXPRESSION:
                    printf(ANSI_COLOR_BRIGHT_GREEN "\n%sRECOGNIZED RULE: reassignment for local IDENTIFIER '%s = %s' in scope '%s' \n" ANSI_COLOR_RESET, indentation, varDeclInstruction->itemName, varDeclInstruction->arrayNode->elements[1]->right->value, scope);
                    varDeclInstruction->arrayNode->elements[0]->left = identifier_node;
                    break;
                default:
                    break;
            }
            
            //Get the assinmnet node ref
            struct ASTNode* assignment_node = GetAssignmentNodeRef(head, varDeclInstruction->itemName, scope);
            printf(ANSI_COLOR_GREY "\n%s--> Info: local IDENTIFIER '%s' in scope '%s' has assignment node already. Assignment node pointer -> '%p'. Value was '%s' and now is '%s' " ANSI_COLOR_RESET, indentation, varDeclInstruction->itemName, scope, assignment_node, assignment_node->right->value, str);
            
            //strcpy(assignment_node->right->value, str);
            assignment_node->right = varDeclInstruction->arrayNode->elements[1];

            // Set the the identifier to the left reference  of the assignment node. This is what needed to be resolved during scope management.
            //varDeclInstruction->arrayNode->elements[1]->left = identifier_node;

            // reset the scope
            strcpy(scope, tempScope);


            // Delete the Assignment node created while parsing. This is always made independent of whether there is a preexisting assignment node due to scope management.
            #if PRINT_INTERMIDIATE_AST == 1
                printf(ANSI_COLOR_BRIGHT_YELLOW "\n#####- ASTNode tree for function '%s' -##### \n\n" ANSI_COLOR_RESET, scope);
                printAST(functionRootNode, 0, 0);
            #endif

            printf(ANSI_COLOR_GREY "\n%s--> Removing assignmentNode created during parsing since assignment already exists. FunctionRootNode -> '%p' is type '%d' \n\n" ANSI_COLOR_RESET, indentation, functionRootNode, functionRootNode->node_type);
            //DeleteASTNode(functionRootNode, GetParentOfNode(functionRootNode, varDeclInstruction->arrayNode->elements[0]));
            //DeleteASTNode(functionRootNode, varDeclInstruction->arrayNode->elements[0]);

            //---------------------------------------------------CHERKCKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!____________ next lkine
            varDeclInstruction->arrayNode->elements[0]->node_type = NODE_BLOCK_PARSE;

            // Run an aditional pass to remove any empty statement nodes that were missed. 
            //DeleteEmptyNodesOfType(functionRootNode, functionRootNode, NODE_STMT, true);

            #if PRINT_INTERMIDIATE_AST == 1
                printf(ANSI_COLOR_BRIGHT_YELLOW "\n#####- ASTNode tree for function '%s' after assignmentNode removal -##### \n\n" ANSI_COLOR_RESET, scope);
                printAST(functionRootNode, 0, 0);
            #endif
            
            ShowAllSymTabs(head);
            printAST(functionRootNode, 0, 0);

            identifier_node->isInitialized = true;
            return true;
        }  
	}
	else
	{
		printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: identifier '%s' referenced before decalration. \n" ANSI_COLOR_RESET, varDeclInstruction->itemName);
		return false;
	}

}

// Done
bool Resolve_param_IDENTIFIER(struct Instruction* funcInstruction, char scope[50])
{
    /// summary:
	/// As mentioned in the parser,
	/// The IDENTIFIER for a Param must be previously declared in the symbol table for the given scope. 
	/// Because the SEMANTIC CHECK for requires the scope to be known, it must be processed later.
	///
	/// So, what type of node should a Param be? 
	/// Based on the previous definition, it will always be a reference to a variable in the symbol table.
	/// 
	/// - There is some IDENTIFIER in the symbol table with the same name as the IDENTIFIER in the param, and it has an assignment node.
	///
	/// - This ASTNode will simply be a reference to the assignment node of the variable in the symbol table.
	///
	/// - The reference from this ASTNode will be assigned to the corresponding ASTNode in the function declaration as the scope is resolved.
	///
	/// Based on this outline, we can initalize this node to an essentially an assignment node with a NULL left node, and a NULL right node.
	/// To avoid confusion I will use NODE_PARAM for this node type, although it represents the same structure as the assignment node. 

    printf(ANSI_COLOR_BRIGHT_YELLOW "\n%s#####- Looking through symbol table for identifier '%s' -##### \n" ANSI_COLOR_RESET, indentation, funcInstruction->itemName);

    // copy the scope in case it gets changed by the checkforidentifier function
    char tempScope[50];
    strcpy(tempScope, scope);

    // Check whether the IDENTIFIER in the is in the symbol table for the scope. This should also check the global scope for declarations.
    int inSymTab = CheckForIdentifierInSymbTabIndent(head, funcInstruction->itemName, scope, indentation, true);

    // If the IDENTIFIER is not found in the symbol table for the current scope or the global scope, throw an SEMANTIC error.
    if(inSymTab == 0){ printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Var %s was not in the current scope '%s' \n" ANSI_COLOR_RESET, funcInstruction->itemName, scope); return false; }
    else if (inSymTab == 1)
    {
        // Get the IDENTIFIER Var ASTNode reference, and whether it has an assignment node reference
        struct ASTNode* assignmentNode = GetAssignmentNodeRef(head, funcInstruction->itemName, scope);  
        if(assignmentNode == NULL) { printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Var %s has not been assigned any value '%p' \n" ANSI_COLOR_RESET, funcInstruction->itemName, funcInstruction->assignmentNodeRef); return false;}

        // If the IDENTIFIER has an assignment node reference, then copy the left and right nodes of the assignment node to the param node of the ast tree created while parsing.
        printf(ANSI_COLOR_GREY"%s--> Info: param IDENTIFIER '%s' in scope '%s' has Assignment node '%p' with value '%s'." ANSI_COLOR_RESET, indentation, funcInstruction->itemName, scope, assignmentNode, assignmentNode->right->value);
        printf(ANSI_COLOR_GREY"\n%s--> Info: Assigning the assignment node to the previously created param node." ANSI_COLOR_RESET, indentation);


        funcInstruction->arrayNode->elements[0]->left = assignmentNode->left;
        funcInstruction->arrayNode->elements[0]->right = assignmentNode->right;

        printf(ANSI_COLOR_GREY"\n%s--> Info: The updated param node left-> '%p', and the updated param node right-> '%p'." ANSI_COLOR_RESET, indentation, funcInstruction->arrayNode->elements[0]->left, funcInstruction->arrayNode->elements[0]->right);
        
        // reset the scope
        strcpy(scope, tempScope);

        // Additional formatting
        printf("\n");

        return true;
    }

   
}


bool Resolve_NonAssignExpr_IDENTIFIER(struct Instruction* instruction, char scope[50])
{
    printf(ANSI_COLOR_BRIGHT_YELLOW "     #####- Looking through symbol table for identifier '%s' -##### \n" ANSI_COLOR_RESET, instruction->itemName);


    // copy the scope in case it gets changed by the checkforidentifier function
    char tempScope[50];
    strcpy(tempScope, scope);

	int inSymTab = CheckForIdentifierInSymbTabIndent(head, instruction->itemName, scope, indentation, true);

    // Dead Code Handling Increment when the indentifier is found
    int update_eval = TryIncrementVarNumRef(head, instruction->itemName, scope);
    if(update_eval == 1){ printf(ANSI_COLOR_ORANGE "\n%s IDENTIFIER REFERENCE: IDENTIFIER '%s' references increased by 1" ANSI_COLOR_RESET, indentation, instruction->itemName); }
    else { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: Var %s was not updated with new number of references\n" ANSI_COLOR_RESET, instruction->itemName); return false;}

    if(inSymTab == 0){ printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Var %s was not in the current scope '%s' \n" ANSI_COLOR_RESET, instruction->itemName, scope); return false; }
    else if (inSymTab == 1)
    {
        char currentValue[50];
       
        
        struct ASTNode* assignmentNode = GetAssignmentNodeRef(head, instruction->itemName, scope);  
        if(assignmentNode != NULL && assignmentNode->right->value != NULL || strcmp(assignmentNode->right->value, "null") || strcmp(assignmentNode->right->value, "Null"))
        {
            strcpy(currentValue, assignmentNode->right->value);

            //Get whether the idetifier kind is param
            char* identifierKind = GetIDENTIFIERKind(head, instruction->itemName, scope, true);

            if(strcmp(identifierKind, "Param") != 0)
            {
                // Dead Code Handling Increment when the indentifier is found
                int update_eval = TryDecrementVarNumRef(head, instruction->itemName, scope);
                if(update_eval == 1){ printf(ANSI_COLOR_ORANGE "\n%s IDENTIFIER REFERENCE: IDENTIFIER '%s' references decreased by 1. Assignment Node value -> '%s' " ANSI_COLOR_RESET, indentation, instruction->itemName, assignmentNode->right->value);  }
                else { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: Var %s was not updated with new number of references\n" ANSI_COLOR_RESET, instruction->itemName); return false;}
            }
        }

        printf(ANSI_COLOR_BRIGHT_GREEN "\n%s RECOGNIZED RULE: nonAssignExpr -> local variable '%s' in scope '%s' (Constant Node val '%s', Assignment Node val '%s') " ANSI_COLOR_RESET, indentation, instruction->itemName, scope, instruction->arrayNode->elements[1]->right->value, assignmentNode->right->value); 
        strncpy(instruction->arrayNode->elements[1]->right->value, currentValue, 50);
        printf(ANSI_COLOR_GREY "\n%s--> Info: The dataType of the nonAssignExpr was '%d' and the assignment node constant is '%d' " ANSI_COLOR_RESET, indentation, instruction->arrayNode->elements[1]->right->nodeData.dataType, assignmentNode->right->nodeData.dataType);


        // Get reference to the identifier variable from the symbol table for the assignment node. 
        struct ASTNode* identifierASTNode = GetASTNodeRef(head, instruction->itemName, scope, true,indentation);
        if(identifierASTNode->node_type == NODE_ASSIGNMENT)
        {
            strcpy(currentValue, identifierASTNode->right->value);
        }
        


        // Get reference to the identifer type from the symbol table
        char* identifierTypeString = GetIDENTIFIERType(head, instruction->itemName, scope, true);
        DataType identifierDataType = CastStringToDataType(identifierTypeString);

       

        if(assignmentNode->left->nodeData.dataType == NULL_TYPE)
        {
             printf(ANSI_COLOR_GREY "\n%s--> Info: The dataType of the identifier was set to NULL_TYPE. Initalizing it now." ANSI_COLOR_RESET, indentation);
            
        }

        // Set the variable node
        instruction->arrayNode->elements[1]->left = assignmentNode->left;

        // Set the constant node reference data type to the identifier data type
        assignmentNode->left->nodeData.dataType = identifierDataType;
        
        // Copy over the current value of the variable         
        strcpy(instruction->arrayNode->elements[1]->right->value, currentValue);

        printf(ANSI_COLOR_BRIGHT_YELLOW "\n%s #####- AST for the identifier Node '%s' -##### \n" ANSI_COLOR_RESET, indentation, instruction->itemName);
        printAST(instruction->arrayNode->elements[1],3,3);
    }

    // reset the scope
    strcpy(scope, tempScope);
    return true;
}

// Done
bool Resolve_IDENTIFIER_OPERATOR_NonAssignExpr(struct Instruction* instruction, char scope[50], struct ASTNode* functionRootNode)
{
    /// summary:
    /// array[0]: This stores a reference to the ASTNode holding the expression "IDENTIFIER OPERATOR NonAssignExpr" 
    /// it will be a constant if constant folding is applicable or binaryexpression node if not. In the parser the value of the 
    /// constant node is set to NULL because the scope is unknown at that point. So, here the value of the constant node needs to be set.
    /// array[1]: holds the ASTNode for the NonAssignExpr in "IDENTIFIER OPERATOR NonAssignExpr"
        

    // copy the scope in case it gets changed by the checkforidentifier function
    char tempScope[50];
    strcpy(tempScope, scope);


	struct ASTNode* identifier_node = GetASTNodeRef(head, instruction->itemName, scope, true,indentation);

    // Dead Code Handling Increment when the indentifier is found
	int update_eval = TryIncrementVarNumRef(head, instruction->itemName, scope);
    if(update_eval == 1){ printf(ANSI_COLOR_ORANGE "\n%s IDENTIFIER REFERENCE: IDENTIFIER '%s' references increased by 1" ANSI_COLOR_RESET, indentation, instruction->itemName); }
    else { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: Var %s was not updated with new number of references\n" ANSI_COLOR_RESET, instruction->itemName); return false;}

	printf(ANSI_COLOR_GREY "\n%s--> Resolve: IDENTIFIER %s pointer address: %p"ANSI_COLOR_RESET, "           ", instruction->itemName,(void*)identifier_node);
	

	if(identifier_node != NULL)
	{

		// -------------------------------- Constant propagation Optimization --------------------------------
		
		// Try and get the current value from the symbol table
		char* symtable_value = TryGetVarValue(head, instruction->itemName, scope);

    
		// If we got the value from the symbol table replace the identifier with its value
		if (symtable_value != NULL) 
		{
            // If the identifier is not a parameter kind.
            char* identifierKind = GetIDENTIFIERKind(head, instruction->itemName, scope, true);
            int identifierIsParam = strcmp(identifierKind, "Param");

            char* nonAssignExpr;
            int nonAssignExprIsParam;

            if(instruction->arrayNode->elements[0]->right->node_type == NODE_ASSIGNMENT) 
            {
                nonAssignExpr = GetIDENTIFIERKind(head, instruction->arrayNode->elements[0]->left->value, scope, true);
                nonAssignExprIsParam = strcmp(nonAssignExpr, "Param");
            }

            
            // Properties for constant folding
            char* foldedResult;
            DataType leftType;
            DataType rightType;
            char rightValue[50];

            struct ASTNode* identifierAssignmentNode;


            if(identifierIsParam == 0)
            {
                printf(ANSI_COLOR_GREY "\n%s--> Info: IDENTIFIER %s is of type '%s' in the context of a function. While it can be propogated here to optimize the function return,\n%s it cannot modify the AST for the function else the IRCode will be not accuratley portray binary expressions for parameters\n" ANSI_COLOR_RESET, indentation, instruction->itemName, identifierKind, indentation);
                identifierAssignmentNode = GetAssignmentNodeRef(head, instruction->itemName, scope);

                char constantValue[50];
                strcpy(constantValue, identifierAssignmentNode->right->value);

                //ASTNode
                struct ASTNode* identifierVarNode = identifierAssignmentNode->left;
                struct ASTNode* identifierConstantNode = createConstantNode(constantValue, identifierAssignmentNode->right->nodeData.dataType);
                struct ASTNode* assignmentNode = createNode(identifierVarNode, identifierConstantNode, NODE_ASSIGNMENT);

                instruction->arrayNode->elements[0]->left = assignmentNode;
            }
        
        
			// Replace the identifier with its value
			char actualValue[50];
			strcpy(actualValue, symtable_value);
			printf(ANSI_COLOR_BRIGHT_MAGENTA "\n%s--> Resolve: Constant Propogation: Setting '%s' to '%s' " ANSI_COLOR_RESET, "           ", instruction->itemName, actualValue);
            

            switch(instruction->arrayNode->elements[0]->node_type)
            {
                case NODE_BINARY_EXPRESSION:
                    // Constant Node is stored in the ASTNode right pointer for binary expression node.
                    char constantValue[50];
                    strcpy(constantValue, instruction->arrayNode->elements[0]->right->value);
                    printf(ANSI_COLOR_GREY "\n%s--> Info: The expression ASTNode Type is Binary Expression. The constant node value is '%s' and the operator is '%s'" ANSI_COLOR_RESET, indentation, constantValue, instruction->arrayNode->elements[0]->type);
                    strcpy(instruction->arrayNode->elements[0]->left->value, actualValue);

                    break;
                default:
                    break;
            }


           

			// -------------------------------- Constant folding Optimization --------------------------------
            /// summary:
            /// If either IDENTIFIER is a param type, still constant fold to get the value, but dont constant fold the astnode itself. 
            /// How do we check if the IDENTIFIER or NonAssignExpr is a param type?
            /// - for the identifier we can use the SymbolTableManager function GetIdentifierKind() and strcmp the result to "param"
            /// - for the right identifier if node is still a BinaryExpression, then we can assume that the right node is a param type. 
            //    for the case of a single identifier, we would check for constant node type, then again strcmmp getIdentifierKind().

    
            // Catch errors
            if(instruction->arrayNode->elements[1] == NULL) { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: child ASTNode reference is NULL\n" ANSI_COLOR_RESET, instruction->itemName); return false; }

            // Get the data type for the right and left side of the expression.
            if(instruction->arrayNode->elements[1]->node_type == NODE_CONSTANT)
            {   
                printf("\n%s #####- AST for the right NODE_CONSTANT  -##### \n" ANSI_COLOR_RESET, indentation);
                printAST(instruction->arrayNode->elements[1],3,3);
                // data type is stored in the ast node for constants
                rightType = instruction->arrayNode->elements[1]->nodeData.dataType;
                if(instruction && instruction->arrayNode->elements[1] && instruction->arrayNode->elements[1]->value != NULL)
                {
                    strcpy(rightValue, instruction->arrayNode->elements[1]->value);
                    printf(ANSI_COLOR_GREY "\n%s--> Info: The data type of the right side of the expression is '%d' and value is '%s' " ANSI_COLOR_RESET, indentation, rightType, rightValue);
                } 
            }
            else if(instruction->arrayNode->elements[1]->node_type == NODE_FUNCTION_CALL)
            {
                // data type is stored in the ast node right child (which is a constant) for function call
                rightType = instruction->arrayNode->elements[1]->right->nodeData.dataType;
                if(instruction && instruction->arrayNode->elements[1] && instruction->arrayNode->elements[1]->right && instruction->arrayNode->elements[1]->right->value != NULL)
                {
                    strcpy(rightValue, instruction->arrayNode->elements[1]->right->value);
                    printf(ANSI_COLOR_GREY "\n%s--> Info: The data type of the right side of the expression is '%d' and value is '%s' " ANSI_COLOR_RESET, indentation, rightType, rightValue);
                }
            }
            else if (instruction->arrayNode->elements[1]->node_type == NODE_BINARY_EXPRESSION)
            {
                printf("\n%s #####- AST for the right NODE_BINARY_EXPRESSION  -##### \n" ANSI_COLOR_RESET, indentation);
                rightType = instruction->arrayNode->elements[1]->left->left->nodeData.dataType;
                printf(ANSI_COLOR_GREY "\n%s --> Target Info: name: %s Datatype = %d type: %s\n" ANSI_COLOR_RESET, indentation, instruction->arrayNode->elements[1]->left->left->value, rightType,  instruction->arrayNode->elements[1]->left->left->type);
                printAST(instruction->arrayNode->elements[1], 3,3);
               
                strcpy(rightValue, instruction->arrayNode->elements[1]->value);
                //exit(1);
            }
            else if(instruction->arrayNode->elements[1]->node_type == NODE_ASSIGNMENT)
            {
                printf("\n%s #####- AST for the right NODE_ASSIGNMENT  -##### \n" ANSI_COLOR_RESET, indentation);
                rightType = instruction->arrayNode->elements[1]->left->nodeData.dataType;
                printf(ANSI_COLOR_GREY "\n%s --> Target Info: name: %s Datatype = %d type: %s\n" ANSI_COLOR_RESET, indentation, instruction->arrayNode->elements[1]->left->value, rightType,  instruction->arrayNode->elements[1]->left->type);
                printAST(instruction->arrayNode->elements[1],3,3);
                // data type is stored in the ast node right for NODE_ASSIGNMENT
                

                if(instruction && instruction->arrayNode->elements[1] && & instruction->arrayNode->elements[1]->right && instruction->arrayNode->elements[1]->right->value != NULL)
                {
                    strcpy(rightValue, instruction->arrayNode->elements[1]->right->value);
                    printf(ANSI_COLOR_GREY "\n%s--> Info: For the assignemnt node, the data type of the right side of the expression is '%d' and value is '%s' " ANSI_COLOR_RESET, indentation, rightType, rightValue);
                } 
            }
            else
            {
                printf(ANSI_COLOR_BRIGHT_RED "\nERROR: unrecognized expression right node type \n" ANSI_COLOR_RESET, instruction->itemName); 
                return false;
            }

            // For Identifier's we need to get the type from the symbol table then cast that to the datatype enum
            printf(ANSI_COLOR_GREY "\n%s--> Info: Trying to grab IDENTIFIER '%s' type in the current scope '%s'. Do check global scope. " ANSI_COLOR_RESET, indentation, instruction->itemName, scope);
            char* leftTypeString = GetIDENTIFIERType(head, instruction->itemName, scope, true);
            leftType = CastStringToDataType(leftTypeString);

            if(leftType == NULL_TYPE || rightType == NULL_TYPE) {printf(ANSI_COLOR_BRIGHT_RED"\n Error Data Type not set [left -> '%s'] [right -> '%s']" ANSI_COLOR_RESET, leftType == NULL_TYPE ? "NULL_TYPE" : "Not Null", rightType == NULL_TYPE ? "NULL_TYPE" : "Not Null"); return false;} 

            // Now that we have both data types we can create the propogation identifiers
            struct PropogationIdentifier* left = initPropogationIdentifier(actualValue, leftType);
            struct PropogationIdentifier* right = initPropogationIdentifier(rightValue, rightType);

            // And then perform the constant folding
            foldedResult = PerformConstantFolding(left, right, instruction->itemKind);

            // Print a message to the console
            printf(ANSI_COLOR_BRIGHT_MAGENTA "\n%s--> Resolve: Constant folded [left type '%d'] [right type '%d']: '%s' %s '%s' = '%s' " ANSI_COLOR_RESET,indentation, leftType, rightType, actualValue, instruction->itemKind, rightValue, foldedResult);

            // copy the folded value to the symbol table 
            strcpy(instruction->arrayNode->elements[0]->value, foldedResult);
            
            // If the identifier is not a parameter kind, then the binary expression can be folded. So, change the type of the node to constant and set the value.
            if(identifierIsParam != 0 && instruction->arrayNode->elements[0]->right->node_type != NODE_BINARY_EXPRESSION && nonAssignExprIsParam != 0)
            {
                // Dead Code Handling Decrement when the indentifier is found
                int update_eval = TryDecrementVarNumRef(head, instruction->itemName, scope);
                if(update_eval == 1){ printf(ANSI_COLOR_ORANGE "\n%s IDENTIFIER REFERENCE: IDENTIFIER '%s' references decreased by 1. Assignment Node value -> '%s' " ANSI_COLOR_RESET, indentation, instruction->itemName, symtable_value); }
                else { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: Var %s was not updated with new number of references\n" ANSI_COLOR_RESET, instruction->itemName); return false;}

            


                printf(ANSI_COLOR_GREY "\n%s--> Info: IDENTIFIER %s and nonAssignExpr is not of type Param in the context of a function. So, change the type of the node to constant and set the value" ANSI_COLOR_RESET, indentation, instruction->itemName);
                instruction->arrayNode->elements[0]->node_type = NODE_CONSTANT;
                strcpy(instruction->arrayNode->elements[0]->value, foldedResult);
                instruction->arrayNode->elements[0]->left = NULL;
                instruction->arrayNode->elements[0]->right = NULL;
                //identifierAssignmentNode = GetAssignmentNodeRef(head, instruction->itemName, scope);
                //instruction->arrayNode->elements[0]->left = identifierAssignmentNode;
            }
        


            printf(ANSI_COLOR_GREY "\n%s--> Info: temp scope '%s', current scope '%s'. Return back to temp scope. " ANSI_COLOR_RESET, indentation, tempScope, scope);

            // Update the dataType of the folded constant node. 
            instruction->arrayNode->elements[0]->nodeData.dataType = leftType; 

            // reset the scope
            strcpy(scope, tempScope);

            printf("\n");
            ShowAllSymTabs(head);

            printAST(functionRootNode,0,0);

            return true;
        }
	}
	else
	{
		printf(ANSI_COLOR_BRIGHT_RED"\n SEMANTIC ERROR: identifier '%s' not found\n"ANSI_COLOR_RESET, instruction->itemName);
        return false;
	}
}

// Done
bool Resolve_NUMBER_OPERATOR_NonAssignExpr(struct Instruction* instruction, char scope[50])
{
    printf(ANSI_COLOR_GREY "%s--> Trying to resolve NUMBER OPERATOR NonAssignExpr " ANSI_COLOR_RESET, indentation);






    // copy the scope in case it gets changed by the checkforidentifier function
    char tempScope[50];
    strcpy(tempScope, scope);


    


    // Need to get whether the right side "NonAssignExpr" is a param, or if it can be folded
    // If the identifier is not a parameter kind.
    char* nonAssignExpr;
    int nonAssignExprIsParam = 1;

    if(instruction->arrayNode->elements[0]->right->node_type == NODE_ASSIGNMENT) 
    {
       nonAssignExpr = GetIDENTIFIERKind(head, instruction->arrayNode->elements[0]->left->value, scope, true);
       nonAssignExprIsParam = strcmp(nonAssignExpr, "Param");
    }

   
   
    struct ASTNode* identifierAssignmentNode;
            
    /*
    if(identifierIsParam == 0)
    {
        printf(ANSI_COLOR_GREY "\n%s--> Info: IDENTIFIER %s is of type '%s' in the context of a function. While it can be propogated here to optimize the function return,\n%s it cannot modify the AST for the function else the IRCode will be not accuratley portray binary expressions for parameters\n" ANSI_COLOR_RESET, indentation, instruction->itemName, identifierKind, indentation);
        identifierAssignmentNode = GetAssignmentNodeRef(head, instruction->itemName, scope);
        instruction->arrayNode->elements[0]->left = identifierAssignmentNode;

        if(instruction->arrayNode->elements[0]->left->tempVar == NULL)
        {
            char *newTemp = generateNewTempVarForParam();
            instruction->arrayNode->elements[0]->left->tempVar = newTemp;
        } 
    }
    */
    












    // -------------------------------- Constant folding Optimization --------------------------------
    /// summary:
    /// If either IDENTIFIER is a param type, still constant fold to get the value, but dont constant fold the astnode itself. 
    /// How do we check if the IDENTIFIER or NonAssignExpr is a param type?
    /// - for the identifier we can use the SymbolTableManager function GetIdentifierKind() and strcmp the result to "param"
    /// - for the right identifier if node is still a BinaryExpression, then we can assume that the right node is a param type. 
    //    for the case of a single identifier, we would check for constant node type, then again strcmmp getIdentifierKind().


    
    // Replace the identifier with its value
    
   
    
    char* foldedResult;
    DataType leftType;
    DataType rightType;

    char leftValue[50];
    char rightValue[50];

    int arrayIndex = atoi(instruction->itemType);

    // Handle grabbing the left expression value. If it is an array, then get the value from looking up the array. 
    switch(instruction->arrayNode->elements[2]->nodeData.dataType)
    {
        case ARRAY_TEMP_ELEMENT:
            struct ASTNode* arrayASTNode = GetASTNodeRef(head, instruction->itemName, scope, true,indentation);
            strcpy(leftValue, arrayASTNode->nodeData.arrayNode->elements[arrayIndex]->right->value);
            printf(ANSI_COLOR_BRIGHT_MAGENTA "\n%s--> Resolve: Constant Propogation: Setting %s[%d] to '%s' " ANSI_COLOR_RESET, "           ", instruction->itemName, arrayIndex, leftValue);
            printf("\n%s #####- AST for the left Array Element -##### \n" ANSI_COLOR_RESET, indentation);
            printAST(arrayASTNode->nodeData.arrayNode->elements[arrayIndex],3,3);

            leftType = arrayASTNode->nodeData.arrayNode->elements[arrayIndex]->nodeData.dataType;
            printf(ANSI_COLOR_GREY "\n%s--> Info: The dataType of array element is '%d' and value is '%s' " ANSI_COLOR_RESET, indentation, leftType, leftValue);
            break;
        default:
            strcpy(leftValue, instruction->arrayNode->elements[2]->value);
            leftType = instruction->arrayNode->elements[2]->nodeData.dataType;
            break;
    }

   
    printf("\n%s #####- AST for the nonAssignExpr  -##### \n" ANSI_COLOR_RESET, indentation);
    printAST(instruction->arrayNode->elements[1],3,3);

    // Catch errors
    if(instruction->arrayNode->elements[1] == NULL) { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: child ASTNode reference is NULL\n" ANSI_COLOR_RESET, instruction->itemName); return false; }

    // Get the data type for the right and left side of the expression.
    if(instruction->arrayNode->elements[1]->node_type == NODE_CONSTANT)
    {   
        // data type is stored in the ast node for constants
        rightType = instruction->arrayNode->elements[1]->nodeData.dataType;
        if(instruction && instruction->arrayNode->elements[1] && instruction->arrayNode->elements[1]->value != NULL)
        {
            strcpy(rightValue, instruction->arrayNode->elements[1]->value);
            printf(ANSI_COLOR_GREY "\n%s--> Info: The data type of the right side of the expression is '%d' and value is '%s' " ANSI_COLOR_RESET, indentation, rightType, rightValue);
        } 
    }
    else if(instruction->arrayNode->elements[1]->node_type == NODE_FUNCTION_CALL)
    {
        // data type is stored in the ast node right child (which is a constant) for function call
        rightType = instruction->arrayNode->elements[1]->right->nodeData.dataType;
        if(instruction && instruction->arrayNode->elements[1] && instruction->arrayNode->elements[1]->right && instruction->arrayNode->elements[1]->right->value != NULL)
        {
            strcpy(rightValue, instruction->arrayNode->elements[1]->right->value);
            printf(ANSI_COLOR_GREY "\n%s--> Info: The data type of the right side of the expression is '%d' and value is '%s' " ANSI_COLOR_RESET, indentation, rightType, rightValue);
        }
    }
    else if (instruction->arrayNode->elements[1]->node_type == NODE_BINARY_EXPRESSION)
    {
        printf("\n%s #####- AST for the right NODE_BINARY_EXPRESSION  -##### \n" ANSI_COLOR_RESET, indentation);
        rightType = instruction->arrayNode->elements[1]->left->left->nodeData.dataType;
        printf(ANSI_COLOR_GREY "\n%s --> Target Info: name: %s Datatype = %d type: %s\n" ANSI_COLOR_RESET, indentation, instruction->arrayNode->elements[1]->left->left->value, rightType,  instruction->arrayNode->elements[1]->left->left->type);
        printAST(instruction->arrayNode->elements[1], 3,3);
        
        strcpy(rightValue, instruction->arrayNode->elements[1]->value);
        //exit(1);
    }
    else if(instruction->arrayNode->elements[1]->node_type == NODE_ASSIGNMENT)
    {
        printf("\n%s #####- AST for the right NODE_ASSIGNMENT  -##### \n" ANSI_COLOR_RESET, indentation);
        rightType = instruction->arrayNode->elements[1]->left->nodeData.dataType;
        printf(ANSI_COLOR_GREY "\n%s --> Target Info: name: %s Datatype = %d type: %s\n" ANSI_COLOR_RESET, indentation, instruction->arrayNode->elements[1]->left->value, rightType,  instruction->arrayNode->elements[1]->left->type);
        printAST(instruction->arrayNode->elements[1],3,3);
        // data type is stored in the ast node right for NODE_ASSIGNMENT
        

        if(instruction && instruction->arrayNode->elements[1] && & instruction->arrayNode->elements[1]->right && instruction->arrayNode->elements[1]->right->value != NULL)
        {
            strcpy(rightValue, instruction->arrayNode->elements[1]->right->value);
            printf(ANSI_COLOR_GREY "\n%s--> Info: For the assignemnt node, the data type of the right side of the expression is '%d' and value is '%s' " ANSI_COLOR_RESET, indentation, rightType, rightValue);
        } 
    }
    else
    {
        printf(ANSI_COLOR_BRIGHT_RED "\nERROR: unrecognized expression right node type \n" ANSI_COLOR_RESET, instruction->itemName); 
        return false;
    }




    if(leftType == NULL_TYPE || rightType == NULL_TYPE) {printf(ANSI_COLOR_BRIGHT_RED"\n Error Data Type not set [left -> '%s'] [right -> '%s']" ANSI_COLOR_RESET, leftType == NULL_TYPE ? "NULL_TYPE" : "Not Null", rightType == NULL_TYPE ? "NULL_TYPE" : "Not Null"); return false;} 

    // Now that we have both data types we can create the propogation identifiers
    struct PropogationIdentifier* left = initPropogationIdentifier(leftValue, leftType);
    struct PropogationIdentifier* right = initPropogationIdentifier(rightValue, rightType);

    // And then perform the constant folding
    foldedResult = PerformConstantFolding(left, right, instruction->itemKind);

    // Print a message to the console
    printf(ANSI_COLOR_BRIGHT_MAGENTA "\n%s--> Resolve: Constant folded [left type '%d'] [right type '%d']: '%s' %s '%s' = '%s' \n" ANSI_COLOR_RESET,indentation, leftType, rightType, leftValue, instruction->itemKind, rightValue, foldedResult);


    // copy the folded value to the symbol table 
    strcpy(instruction->arrayNode->elements[0]->value, foldedResult);
    


    // If the identifier is not a parameter kind, then the binary expression can be folded. So, change the type of the node to constant and set the value.
    if(instruction->arrayNode->elements[0]->right->node_type != NODE_BINARY_EXPRESSION && nonAssignExprIsParam != 0)
    {
        printf(ANSI_COLOR_GREY "\n%s--> Info: NonAssignExpr is not of type Param in the context of a function.So, change the type of the node to constant and set the value" ANSI_COLOR_RESET, indentation);
        instruction->arrayNode->elements[0]->node_type = NODE_CONSTANT;
        strcpy(instruction->arrayNode->elements[0]->value, foldedResult);
        instruction->arrayNode->elements[0]->left = NULL;
        instruction->arrayNode->elements[0]->right = NULL;

        //identifierAssignmentNode = GetAssignmentNodeRef(head, instruction->itemName, scope);
        //instruction->arrayNode->elements[0]->left = identifierAssignmentNode;
    }

    // reset the scope
    strcpy(scope, tempScope);

   return true;

}

// Done
bool Resolve_IDENTIFIER_LPAR_ParamList_RPAR(struct Instruction* funcInstruction, char scope[50])
{

    /// summary:
    /// This is the final revison. So, the function call works fine as long as there are no live IDENTIFIERS in the function. 
    /// To handle live IDENTIFIERS, the data related to that specific instance needs to be stored somehow. Here is an example of what the end goal is:
    ///
    ///        .data
    ///        newline:    .asciiz "\n"
    ///        output:     .asciiz "You entered: "
    ///
    ///    .text
    ///        # Function to print an integer
    ///        print_int:
    ///            # Print output message
    ///            li $v0, 4
    ///            la $a0, output
    ///            syscall
    ///
    ///            # Print integer
    ///            li $v0, 1
    ///            move $a0, $t0   # Pass integer value to print
    ///            syscall
    ///
    ///            # Print newline
    ///            li $v0, 4
    ///            la $a0, newline
    ///           syscall
    ///
    ///            jr $ra          # Return to caller
    ///
    ///        main:
    ///            # Display prompt
    ///            li $v0, 4
    ///            la $a0, prompt
    ///            syscall
    ///
    ///            # Read integer from user
    ///            li $v0, 5
    ///            syscall
    ///            move $t0, $v0   # Move user input to $t0 register
    ///
    ///            # Jump to function
    ///            j print_int
    ///
    ///            # Exit program
    ///            li $v0, 10
    ///            syscall
    ///
    /// Key takeaways: 
    ///  - any live parameters need to moved to a register before the jump to the function
    ///  - any instructions associated with a live function instruction need to be included within the function itself
    /// The main question is whether the those properties can be derived from just the instruction symbol table, or whether 
    /// it needs to be done after the live analysis.
    ///  - It looks like besides parameters, the liveliness of IDENTIFIERS is accurate.
    ///
    /// Advantages of doing it before the live analysis:
    /// - As the functioncall is being resolved, we can set the necessary properties directly into some struct for the function
    /// - On the otherhand, if it had to be done after the liveliness analysis, I think then either the function would need to processed again,
    ///    or we would have to store reference to every instruction value in the function call, becuase we can't rule out what is and what is not impotant.
    /// 
    /// Assuming it can be done on the first call of the function, after the function instructions are processed, we would extract the 
    /// live IDENTIFIERS and their current values. 
    /// So jump to the end of thus function for the implementation...

    /// summary:
    /// The first thing that needs to be done is clear any values in the function symbol table if the table is not empty. This is what allows for multiple function calls. 
    ResetSymbolTable(head, funcInstruction->itemName);
    ShowAllSymTabs(head);


    /// summary:
    /// Now process the function itself, and copy the return value to this assignment statement.
    /// What I have done is changed the way that the function is processed. Instead of processing the function, 
    /// when the scope is known, Im going to push the instructions into an instruction symbol linked to the function's main symbol table.
    /// This way, each time I call the function, I can assign the parameters, then process the function instructions, and then copy the return value to the assignment node.
    
    
    printf(ANSI_COLOR_BRIGHT_GREEN "\nRECOGNIZED RULE: Function Call Declared, Copying Function Param Instructions To Stack. Func -> '%s'\n" ANSI_COLOR_RESET, funcInstruction->itemName);
    printf(ANSI_COLOR_GREY "\n%s--> Info: function call made from scope '%s'. Target scope is '%s' \n" ANSI_COLOR_RESET, indentation, scope, funcInstruction->itemName);
    PopFuncInstruction();

    // Get reference to the function root node created while parsing
    struct ASTNode* functionRootNode = GetASTNodeRef(head, funcInstruction->itemName, globScope, false, indentation);

    printf(ANSI_COLOR_BRIGHT_YELLOW "\n%s#####- Stack Order Before Function Copy -##### \n" ANSI_COLOR_RESET, indentation);
    DisplayFuncInstructionStack();

    /// So, copy all of the instructions in the associated with the function call to the stack to be proccessed.
    struct SymbolTableNode* funcSymTabNode = GetSymbolTableFromLLScope(head, funcInstruction->itemName);
    if(funcSymTabNode == NULL) { printf(ANSI_COLOR_RED "ERROR: function table is NULL. Head is '%p', scope is '%s' \n" ANSI_COLOR_RESET, head, funcInstruction->itemName); return; }
    struct SymbolTableNode* instructionTable = funcSymTabNode->instructionTable;
    if(funcSymTabNode->instructionTable == NULL) { printf(ANSI_COLOR_RED "ERROR: instructionSymTabNode is NULL \n" ANSI_COLOR_RESET); return; }

    PushFuncInstruction(CreateBreakInstruction());
    copyParamInstructionsToStack(instructionTable);
    printf(ANSI_COLOR_BRIGHT_YELLOW "\n%s#####- Stack Order After Param Copy -##### \n" ANSI_COLOR_RESET, indentation);
    DisplayFuncInstructionStack();

    ReverseFuncInstructionStack();
    ResolveFunctionScope(funcInstruction->itemName, functionRootNode);
    
    
    
    /// summary:
    /// Check whether the IDENTIFIER exists in the symbol table. If it does, check whether it is a function.
    /// Check whether the parameters passed in the function call match the parameters of the function in the symbol table.
    /// 
    /// I can think of two ways to handle functions. One, if a function outputs some constant, then the function itself can be constant folded.
    /// If the the function has some other output, such as a write statement, then the function can't be entirely optimized this way.
    ///
    /// To handle passing parameters, take the values in the paramList and set the coresponding IDENTIFIER param in the function scope to that value.
    /// More specifically, we would take the Assignment Node for each IDENTIFIER in the paramList and assign it to the Assignment Node for the IDENTIFIER in the function scope.

    printf(ANSI_COLOR_BRIGHT_GREEN "\n RECOGNIZED RULE CONTINUE: Now that the params have been assigned pushing function instructions to stack\n" ANSI_COLOR_RESET);
    
    printf(ANSI_COLOR_BRIGHT_YELLOW "\n%s#####- Looking through symbol table for identifier '%s' -##### \n" ANSI_COLOR_RESET, indentation, funcInstruction->itemName);

    // copy the scope in case it gets changed by the checkforidentifier function
    char tempScope[50];
    strcpy(tempScope, scope);

    // Check whether the IDENTIFIER in the is in the symbol table for the scope. This should also check the global scope for declarations.
    int inSymTab = CheckForIdentifierInSymbTabIndent(head, funcInstruction->itemName, scope, indentation, true);

    printf(ANSI_COLOR_GREY"\n%s--> Info: Scope -> '%s'. Temp Scope used for storing the base scope -> '%s' " ANSI_COLOR_RESET, indentation, scope, tempScope);

    strcpy(scope, tempScope);

    // If the IDENTIFIER is not found in the symbol table for the current scope or the global scope, throw an SEMANTIC error.
    if(inSymTab == 0){ printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Function %s was not in the current scope '%s' \n" ANSI_COLOR_RESET, funcInstruction->itemName, scope); return false; }
    else if (inSymTab == 1)
    {
        printf(ANSI_COLOR_GREY"\n%s--> Info: Function was found. Setting the current scope to '%s'." ANSI_COLOR_RESET, indentation, funcInstruction->itemName);
        strcpy(scope, funcInstruction->itemName);

        /// summary:
        /// Parameters in the function scope are guarenteed to be the first items in the symbol table for the function scope, 
        /// ordered by the order they were declared. So left to right becomes first to last. Using this fact, we can set the 
        /// values of the parameters in the function scope to the values of the parameters passed in the function call.

        // Get the number of parameters in the function scope
        int numParams = GetNumParamsInFuncSymTab(head, scope);

        // Get the number of parameters passed in the function call
        int numParamsPassed = CountInstances(funcInstruction->arrayNode->elements[1], NODE_PARAM);

        // If the number of parameters passed in the function call does not match the number of parameters in the function scope, throw an SEMANTIC error.
        if(numParams != numParamsPassed) { 
            printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Function %s was passed '%d' parameters, but it has '%d' parameters in the function scope '%s' \n" ANSI_COLOR_RESET, funcInstruction->itemName, numParamsPassed, numParams, scope); \
            return false; 
        }
        else { 
            printf(ANSI_COLOR_YELLOW"\n%s--> Info: Semantic check passed. Function '%s' was passed '%d' parameters, and it has '%d' parameters in the function scope '%s'." ANSI_COLOR_RESET, indentation, funcInstruction->itemName, numParamsPassed, numParams, scope); 
        }

        for(int i = 0; i < numParamsPassed; i++) 
        {
            //Get the type of the PARAM in the function declartion
            char* paramType = GetParamType(head, scope, i);
            char* paramName = GetIdentifierName(head, scope, i);

            printf(ANSI_COLOR_GREY "\n%s--> Info: The type of the param in the function declaration is '%s'." ANSI_COLOR_RESET, indentation, paramType);

            int count = 0;
            struct ASTNode* paramNode = getNthNodeOfType(funcInstruction->arrayNode->elements[1], NODE_PARAM, &count, i+1);

            if (paramNode != NULL) {
                printf(ANSI_COLOR_GREY"\n%s--> Info: Found NODE_PARAM: '%s'" ANSI_COLOR_RESET,indentation, paramNode->left->value);
            } else {
                printf(ANSI_COLOR_BRIGHT_RED "\n ERROR: No matching paramNode found while comparing types.\n" ANSI_COLOR_RESET);
            }

            // Check against the Type of the PARAM in the function call (as stated previosuly this Param Node is essentially an assignment node)
            if(strcmp(paramType, paramNode->left->type) != 0) { 
                printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Function %s was passed a parameter of type '%s', but it has a parameter of type '%s' in the function scope '%s' \n" ANSI_COLOR_RESET, funcInstruction->itemName, paramNode->left->type, paramType, scope); 
                return false; 
            }
            else { 
                printf(ANSI_COLOR_YELLOW"\n%s--> Info: Semantic check passed. Function '%s' was passed a parameter of type '%s', and it has a parameter of type '%s' in the function scope '%s'." ANSI_COLOR_RESET, indentation, funcInstruction->itemName, paramNode->left->type, paramType, scope); 
            }

            // Now that the necessary semantic checks have been passed, we can set the assignment node reference for each param 
            // in the function scope to the assignment node reference for each param in the function call. I am going to do this in the
            // same loop as the semantic checks to avoid having to loop through the function call parameters again.
            // TryUpdateAssignmentNodeRef(struct SymbolTableNode* head, char itemName[50], char scope[50], struct ASTNode* assignment_node)

            // Create a new assignment node for the param in the function scope
            char inputParamValue[50];

            // Get the ASTNode for the param from the symbol table
            struct ASTNode* paramDeclASTNode = GetASTNodeRef(head, paramName, scope, false, indentation);

            // Assign initalization values 
            strcpy(inputParamValue, paramNode->right->value);
            strcpy(paramDeclASTNode->right->value, inputParamValue);
            paramDeclASTNode->left->nodeData.dataType = CastStringToDataType(paramType);


            // If the parameter Node doesnt have a temp param value, then create one.
            if(paramDeclASTNode->left->tempVar == NULL)
            {
                char *newTemp = generateNewTempVar();
                paramDeclASTNode->left->tempVar = newTemp;
                paramNode->left->tempVar = newTemp;
            } 


           
            //strcpy(inputParamValue, paramNode->right->value);
            //struct ASTNode* newParamAssignNode = createNode(createVariableDeclarationNode(paramName,paramType), createConstantNode(inputParamValue, CastStringToDataType(paramType)), NODE_ASSIGNMENT);
            //paramDeclASTNode = newParamAssignNode;

            int updatedAssignment = TryUpdateAssignmentNodeRef(head, paramName, scope, paramDeclASTNode);


            updatedAssignment == 1 ? printf(ANSI_COLOR_GREY"\n%s--> Info: The assignment node reference for param '%s' in scope '%s' was updated to '%p' which has value '%s'.\n" ANSI_COLOR_RESET, indentation, paramName, scope, paramNode, paramNode->right->value) : printf(ANSI_COLOR_BRIGHT_RED "\nERROR: The assignment node reference for param '%s' in scope '%s' was not updated to '%p'." ANSI_COLOR_RESET, paramName, scope, paramNode);
            int updatedVarValue = TryUpdateVarValue(head, paramName, scope, paramNode->right->value);
            updatedVarValue == 1 ? printf(ANSI_COLOR_GREY"\n%s--> Info: The IDENTIFIER '%s' value was updated in the symbol table to '%s' in scope '%s'." ANSI_COLOR_RESET, indentation, paramName, paramNode->right->value, scope) : printf(ANSI_COLOR_BRIGHT_RED"\nERROR: The IDENTIFIER '%s' value in scope '%s' was not updated in the symbol table." ANSI_COLOR_RESET, paramName, scope);

            // Dead code handling
            int update_eval = TryIncrementVarNumRef(head, paramName, funcInstruction->itemName);
            if(update_eval == 1){ printf(ANSI_COLOR_ORANGE "\n IDENTIFIER REFERENCE: IDENTIFIER '%s' references in scope '%s' increased by 1 \n" ANSI_COLOR_RESET, paramName, funcInstruction->itemName); }
            else { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: Var %s was not updated with new number of references\n" ANSI_COLOR_RESET, funcInstruction->itemName); return false;}

            // Set the indetifier to be initialized, this is important for write statements and other stamtents dependent on the parameter.
            struct ASTNode* identifier_node = GetASTNodeRef(head,  paramName, funcInstruction->itemName, false, "");
            identifier_node->isInitialized = true;



            char currentValue[50];
            strcpy(currentValue, paramNode->right->value);

            // Set the function node array of params
            funcInstruction->arrayNode->elements[0]->nodeData.arrayNode->elements[i] = createNode(identifier_node->left, createConstantNode(currentValue, NULL_TYPE), NODE_ASSIGNMENT);

        }

        // reset the scope
        strcpy(scope, tempScope);

        // Additional formatting
        printf("\n");
    }

    
    printf(ANSI_COLOR_BRIGHT_YELLOW "\n%s#####- Stack Order Before Func Instruction Copy -##### \n" ANSI_COLOR_RESET, indentation);
    DisplayFuncInstructionStack();
    
    copyNonParamInstructionsToStack(instructionTable);
    printf(ANSI_COLOR_BRIGHT_YELLOW "\n%s#####- Stack Order After Func Instruction Copy -##### \n" ANSI_COLOR_RESET, indentation);
    DisplayFuncInstructionStack();

    ReverseFuncInstructionStack();
    ResolveFunctionScope(funcInstruction->itemName, functionRootNode);
    
    // copy the current value of the function return to the assignment node
    printf(ANSI_COLOR_BRIGHT_YELLOW "\n%s#####- Copying function return value to assignment node -##### \n" ANSI_COLOR_RESET, indentation);
    
    /// summary:
    /// Need to get reference to the function return node. 
    /// The path to the return node is as follows:
    /// 1.) Get the ASTNode ref from the global symbol table for the function
    /// 2.) The ref->right points to the Block
    /// 3.) The Block->right points to the Return node

    struct ASTNode* functionNode = GetASTNodeRef(head, funcInstruction->itemName, globScope, false, indentation);
    if(functionNode == NULL) { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: functionNode is NULL \n" ANSI_COLOR_RESET); return false; }
    struct ASTNode* returnNode = functionNode->right->right;
    if(returnNode == NULL) { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: returnNode is NULL \n" ANSI_COLOR_RESET); return false; }

    /// copy the value of the return node to the assignment node
    /// The return node itself just holds a reference to the NonAssignExpr
    ///
    /// This is an example of how it has been previously handled:
    /// char str[50];	
    /// strcpy(str, varDeclInstruction->child_ast_node_ref->value);
    /// varDeclInstruction->assignmentNodeRef->right = varDeclInstruction->child_ast_node_ref;

    char str[50];	

    switch(returnNode->right->node_type)
    {
        case NODE_ASSIGNMENT:
            strcpy(str, returnNode->right->right->value);
            char choice;
            break;
        case NODE_CONSTANT:
            strcpy(str, returnNode->right->value);
            
            break;
        default:
            printf(ANSI_COLOR_BRIGHT_RED "\nERROR: unrecognized return node type \n" ANSI_COLOR_RESET, funcInstruction->itemName); 
            exit(1);
            break;
    }
    
    printf(ANSI_COLOR_GREY "\n%s--> Info: The value stored in the returnNode is '%s'." ANSI_COLOR_RESET, indentation, str);

    strcpy(funcInstruction->arrayNode->elements[0]->right->value, str);
    //funcInstruction->arrayNode->elements[0]->right->nodeData.dataType = returnNode->right->nodeData.dataType;
    
    printf(ANSI_COLOR_GREY "\n%s--> Info: The value stored in the ASTNode constant node created while parsing is now '%s'." ANSI_COLOR_RESET, indentation, funcInstruction->arrayNode->elements[0]->right->value);   

    /// summary:
    /// So normally the IRCode is generate by recursive traversal of the AST tree. While the IRCode will contine to be handled that way, 
    /// specifically for function calls, part of the IRCode will be generated by a liveliness analysis of the function. On the first call of the function,
    /// we will have the data necessary to generate the structure of the IRCode for the function. 
    /// Subsequent calls of the function will be swapping values out. 
    ///
    /// So everything can really be broken down currently into two parts:
    /// 1.) A write statement to print the value of some IDENTIFIER.
    /// 2.) The IDENTIFIER itself with is just some constant.
    /// 
    /// So, using this information, we can simplify the IRCode generation for function calls by saying:
    /// a.) Im going to pass a list of constants (order matters in this case)
    /// b.) for each constant in the list I will just assign it to the next availiable register (this probably needs to be changed), 
    ///     I will then generate a print statement in the function call code.
    ///
    ///
    
    printf(ANSI_COLOR_BRIGHT_YELLOW "\n\n%s#####- Generating IRCode for Function Structure. Here is the current symbol table for reference -##### \n" ANSI_COLOR_RESET, indentation);

    ShowAllSymTabs(head);

    int numLiveID = GetNumberOfLiveIDENTIFIERS(head, funcInstruction->itemName, false);
    printf(ANSI_COLOR_GREY "\n%s--> Info: The number of live non-Param IDENTIFIERS in the function '%s' is '%d'." ANSI_COLOR_RESET, indentation, funcInstruction->itemName, numLiveID);


    // Handle IRCode gneration for function calls. 

    if(funcSymTabNode->instructionTable->isInitialized == false)
    {
        printf(ANSI_COLOR_GREY "\n%s--> Info: Opening file to write function '%s' IRCode.\n" ANSI_COLOR_RESET, indentation, funcInstruction->itemName);

        writeFunctionIRCodeToFile(functionRootNode, funcInstruction->itemName);
        funcSymTabNode->instructionTable->isInitialized = true;
    }

    return true;
}


bool Resolve_WRITE_IDENTIFIER(struct Instruction* instruction, char scope[50])
{


    struct ASTNode* identifier_node = GetASTNodeRef(head, instruction->itemName, scope, true, "");

    if(identifier_node == NULL){ printf(ANSI_COLOR_BRIGHT_RED "\nERROR: identifier_node is NULL \n" ANSI_COLOR_RESET); return false; }
    if(!identifier_node->isInitialized){ printf(ANSI_COLOR_BRIGHT_RED "\nERROR: identifier_node is not initialized \n" ANSI_COLOR_RESET); return false; }

    struct ASTNode* assignment_node = GetAssignmentNodeRef(head, instruction->itemName, scope);

    if(assignment_node == NULL){ printf(ANSI_COLOR_BRIGHT_RED "\nERROR: assignment_node is NULL \n" ANSI_COLOR_RESET); return false; }
    
    // Dead code handling
    int update_eval = TryIncrementVarNumRef(head, instruction->itemName, scope);
    if(update_eval == 1){ printf(ANSI_COLOR_ORANGE "\n IDENTIFIER REFERENCE: IDENTIFIER '%s' references increased by 1\n" ANSI_COLOR_RESET, instruction->itemName); }
    else { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: Var %s was not updated with new number of references\n" ANSI_COLOR_RESET, instruction->itemName); return false;}
    
    //Go back and set the right ASTNode of the Write node -> Prev "$$ = createNode(NULL, identifier_node, NODE_WRITE)";
     
    instruction->arrayNode->elements[0]->left = identifier_node;
    char value[50];
    strcpy(value, assignment_node->right->value);

    printf(ANSI_COLOR_GREY "\n%s--> Info: The value stored in the assignment node is for NODE_WRITE '%s'." ANSI_COLOR_RESET, indentation, value);
    instruction->arrayNode->elements[0]->right = assignment_node->right; //createConstantNode(assignment_node->right->value);
    //instruction->ast_node_ref->right = assignment_node->right; 
    //instruction->ast_node_ref->right->nodeData.isFunctionCall = false;
    
    //strcpy(instruction->ast_node_ref->right->value, assignment_node->right->value);

	return true;
}


bool Resolve_ARRAY_DECLARATION(struct Instruction* instruction, char scope[50])
{
    printf(ANSI_COLOR_BRIGHT_YELLOW "           #####- Looking through symbol table for array identifier '%s' -##### \n" ANSI_COLOR_RESET, instruction->itemName);

    // Check whether the IDENTIFIER in the "TYPE IDENTIFIER EQ NonAssignExpr" is in the symbol table for the function scope. 
    // If it does not exist, do not check the global scope since this is a local variable declaration.
	int inSymTab = CheckForIdentifierInSymbTabIndent(head, instruction->itemName, scope, indentation, false);

	if (inSymTab == 0)
	{
		printf(ANSI_COLOR_GREY "%s--> Info: IDENTIFIER array '%s' was not found to be previously declared in the function scope '%s' " ANSI_COLOR_RESET, indentation, instruction->itemName, scope);
        printf(ANSI_COLOR_BRIGHT_GREEN "\n%sRECOGNIZED RULE: declaration for local array variable '%s' initalization in scope '%s' \n" ANSI_COLOR_RESET, 
        indentation, instruction->itemName, scope);
        
        /// summary: 
        /// Add IDENTIFIER to the function symbol table. The declaration of the function for that is as follows:
        /// addItemToFuncSymTab(struct SymbolTableNode* funcSymTabNode, char itemName[50], char itemKind[8], char itemType[8], struct ASTNode* ast_node_ref)
        addItemToFuncSymTab(head, scope, instruction->itemName, instruction->itemKind, instruction->itemType,instruction->arrayNode->elements[0]);
	}
	else
	{
		printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: Var %s is already in the symbol table\n" ANSI_COLOR_RESET, instruction->itemName);
		return false;
	}

    // Incremenet the number of references to the array variable
	int update_eval = TryIncrementVarNumRef(head,instruction->itemName,scope);

    ShowAllSymTabs(head);

    return true;
}



bool Resolve_ARRAY_IDENTIFIER_EQ(struct Instruction* instruction, char scope[50], struct ASTNode* functionRootNode)
{
    /// summary:
    /// Setting the value of an array element should be similiar to that of a normal IDENTIFIER. 
    /// Here are the main differences:
    /// - Assignment node in parser is replaced with array assignment node. The value of that node is the index of the array.
    /// - need to check whether the index is within the bounds of the array.
    /// - need to check whether the type of the value being assigned matches the type of the array.
    /// - we can treat each index as an assignment node, where the left side is "var index"


    /*
        char* foldedResult;
        DataType leftType;
        DataType rightType;

        char leftValue[50];
        char rightValue[50];

        // Handle grabbing the left expression value. If it is an array, then get the value from looking up the array. 
        switch(instruction->arrayNode->elements[2]->nodeData.dataType)
        {
            case ARRAY_TEMP_ELEMENT:
                struct ASTNode* arrayASTNode = GetASTNodeRef(head, instruction->itemName, scope, true,indentation);
                strcpy(leftValue, arrayASTNode->nodeData.arrayNode->elements[atoi(instruction->itemType)]->value);
                printf(ANSI_COLOR_BRIGHT_MAGENTA "\n%s--> Resolve: Constant Propogation: Setting %s[%d] to '%s' " ANSI_COLOR_RESET, "           ", instruction->itemName, instruction->itemType, leftValue);
                break;
            default:
                strcpy(leftValue, instruction->arrayNode->elements[2]->value);
                break;
        }
    */   






    printf(ANSI_COLOR_BRIGHT_YELLOW "           #####- Looking through symbol table for identifier '%s', target scope '%s' -##### \n" ANSI_COLOR_RESET, instruction->itemName, scope);

    // copy the scope in case it gets changed by the checkforidentifier function
    char tempScope[50];
    strcpy(tempScope, scope);

    int targetIndex = atoi(instruction->itemType);
    printf("\n target index %d", targetIndex);
    

    // Check whether the IDENTIFIER in the "IDENTIFIER EQ" is in the symbol table for the scope.
    // This is a SEMANTIC CHECK for whether the IDENTIFIER has been declared before it is used.
    
	int inSymTab = CheckForIdentifierInSymbTabIndent(head, instruction->itemName, scope, indentation, true);

	if (inSymTab == 1)
	{
        printf(ANSI_COLOR_GREY "%s--> Info: IDENTIFIER '%s' was found to be previously declared in the function scope '%s' " ANSI_COLOR_RESET, indentation, instruction->itemName, scope);

        // Get the IDENTIFIER Var ASTNode reference, and whether it has an assignment node reference
        struct ASTNode* identifier_node = GetASTNodeRef(head,  instruction->itemName, scope, true,"");
        //int has_assignment_node = DoesVarAlreadyHaveAssignment(head, instruction->itemName, scope);

        struct ASTNode* arrayElementASTNode;

        switch(identifier_node->node_type) 
        {
            case NODE_ARRAY_ROOT:
                printf(ANSI_COLOR_GREY "\n%s--> Info: IDENTIFIER '%s' in scope '%s''s array element '%d' set to target " ANSI_COLOR_RESET, indentation, instruction->itemName, scope, targetIndex);
                identifier_node->nodeData.arrayNode->elements[targetIndex] = instruction->arrayNode->elements[0];
                printf(ANSI_COLOR_GREY "\n%s--> Info: Pointer for the target is '%p' " ANSI_COLOR_RESET, indentation,identifier_node->nodeData.arrayNode->elements[targetIndex]);
                break;
            default:
                printf(ANSI_COLOR_BRIGHT_RED "\nERROR: IDENTIFIER '%s' is not an array in scope '%s' \n" ANSI_COLOR_RESET, instruction->itemName, scope);
                exit(1);
                break;
        }

        char rightValue[50];
        char rightType[50];
        int updated_var;

        // Handle grabbing the right expression value. If it is an array, then get the value from looking up the array.
        printf(ANSI_COLOR_GREY "\n%s--> Info: Trying to get the right NonAssignExpr value " ANSI_COLOR_RESET, indentation);
        printf(ANSI_COLOR_BRIGHT_YELLOW "\n%s#####- AST for the NonAssignExpr -##### \n" ANSI_COLOR_RESET, indentation);

        printAST(instruction->arrayNode->elements[1], 3,3);
        if(instruction->arrayNode->elements[1]->node_type == NODE_CONSTANT)
        {
            printf(ANSI_COLOR_GREY "\n%s--> Info: NonAssignExpr is a constant " ANSI_COLOR_RESET, indentation);
            strcpy(rightValue, instruction->arrayNode->elements[1]->value);
            DataType rightDataType =  instruction->arrayNode->elements[1]->nodeData.dataType;
            strcpy(rightType, CastDataTypeToString(rightDataType));

            arrayElementASTNode = instruction->arrayNode->elements[0];

            arrayElementASTNode->left->nodeData.dataType = rightDataType;
            strcpy(arrayElementASTNode->left->type, rightType);

            strcpy(arrayElementASTNode->right->value, rightValue);
            

        }
        else
        {
            switch(instruction->arrayNode->elements[1]->right->node_type)
            {
                case NODE_CONSTANT:
                    printf(ANSI_COLOR_BRIGHT_GREEN "\n%sRECOGNIZED RULE: reassignment for local IDENTIFIER '%s[%d] = %s' in scope '%s' \n" ANSI_COLOR_RESET, indentation, instruction->itemName,atoi(instruction->itemType), instruction->arrayNode->elements[1]->right->value, scope);
                    strcpy(rightValue,instruction->arrayNode->elements[1]->right->value);
                    strcpy(rightType,instruction->arrayNode->elements[1]->left->type);
                    break;
                case NODE_FUNCTION_CALL:
                    printf(ANSI_COLOR_BRIGHT_GREEN "\n%sRECOGNIZED RULE: reassignment for local IDENTIFIER '%s[%d] = %s' in scope '%s' \n" ANSI_COLOR_RESET, indentation, instruction->itemName,atoi(instruction->itemType), instruction->arrayNode->elements[1]->right->value, scope);
                    strcpy(rightValue, instruction->arrayNode->elements[1]->right->value);
                    strcpy(rightType,instruction->arrayNode->elements[1]->left->type);
                    break;
                case NODE_BINARY_EXPRESSION:
                    printf(ANSI_COLOR_BRIGHT_GREEN "\n%sRECOGNIZED RULE: reassignment for local IDENTIFIER '%s[%d] = %s' in scope '%s' \n" ANSI_COLOR_RESET, indentation, instruction->itemName,atoi(instruction->itemType), instruction->arrayNode->elements[1]->right->value, scope);
                    instruction->arrayNode->elements[0]->left = identifier_node;
                    break;
                default:
                    printf(ANSI_COLOR_BRIGHT_RED "\nERROR: NonAssignExpr node type '%d' not recognized \n" ANSI_COLOR_RESET, instruction->arrayNode->elements[1]->node_type);
                    exit(1);
                    break;
            }

            arrayElementASTNode = instruction->arrayNode->elements[0];
            strcpy(instruction->arrayNode->elements[0]->right->value, rightValue);
            strcpy(instruction->arrayNode->elements[0]->left->type, rightType);
            instruction->arrayNode->elements[0]->left->nodeData.dataType = CastStringToDataType(identifier_node->type);

            strcpy(arrayElementASTNode->right->value, rightValue);
            strcpy(arrayElementASTNode->left->type, rightType);

        }

      

       
        printf(ANSI_COLOR_BRIGHT_YELLOW "\n%s#####- AST for the Array Element Node -##### \n" ANSI_COLOR_RESET, indentation);
        printAST(arrayElementASTNode, 3,3);

        printf(ANSI_COLOR_BRIGHT_YELLOW "\n%s#####- AST for the Updated Array Node -##### \n" ANSI_COLOR_RESET, indentation);
        printAST(identifier_node, 3,3);

        
        //arrayElementASTNode->right = instruction->arrayNode->elements[1];

        
        
            //Get the assinmnet node ref
            //struct ASTNode* assignment_node = GetAssignmentNodeRef(head, instruction->itemName, scope);
            //printf(ANSI_COLOR_GREY "\n%s--> Info: local IDENTIFIER '%s' in scope '%s' has assignment node already. Assignment node pointer -> '%p'. Value was '%s' and now is '%s' " ANSI_COLOR_RESET, indentation, instruction->itemName, scope, assignment_node, assignment_node->right->value, str);
            
            //strcpy(assignment_node->right->value, str);
           

            // Set the the identifier to the left reference  of the assignment node. This is what needed to be resolved during scope management.
            //varDeclInstruction->arrayNode->elements[1]->left = identifier_node;

            // reset the scope
            strcpy(scope, tempScope);


            // Delete the Assignment node created while parsing. This is always made independent of whether there is a preexisting assignment node due to scope management.
            #if PRINT_INTERMIDIATE_AST == 1
                printf(ANSI_COLOR_BRIGHT_YELLOW "\n#####- ASTNode tree for function '%s' -##### \n\n" ANSI_COLOR_RESET, scope);
                printAST(functionRootNode, 0, 0);
            #endif

            //printf(ANSI_COLOR_GREY "\n%s--> Removing assignmentNode created during parsing since assignment already exists. FunctionRootNode -> '%p' is type '%d' \n\n" ANSI_COLOR_RESET, indentation, functionRootNode, functionRootNode->node_type);
            //DeleteASTNode(functionRootNode, GetParentOfNode(functionRootNode, varDeclInstruction->arrayNode->elements[0]));
            //DeleteASTNode(functionRootNode, varDeclInstruction->arrayNode->elements[0]);
            //instruction->arrayNode->elements[0]->node_type = NODE_BLOCK_PARSE;

            // Run an aditional pass to remove any empty statement nodes that were missed. 
            //DeleteEmptyNodesOfType(functionRootNode, functionRootNode, NODE_STMT, true);

            #if PRINT_INTERMIDIATE_AST == 1
                printf(ANSI_COLOR_BRIGHT_YELLOW "\n#####- ASTNode tree for function '%s' after assignmentNode removal -##### \n\n" ANSI_COLOR_RESET, scope);
                printAST(functionRootNode, 0, 0);
            #endif
            
            ShowAllSymTabs(head);
            //printAST(functionRootNode, 0, 0);


            return true;
        
	}
	else
	{
		printf(ANSI_COLOR_BRIGHT_RED "\nSEMANTIC ERROR: identifier '%s' referenced before decalration. \n" ANSI_COLOR_RESET, instruction->itemName);
		return false;
	}

    return true;
}

bool Resolve_WHILEBLOCK(struct Instruction* instruction, char scope[50], struct ASTNode* functionRootNode)
{
    printf(ANSI_COLOR_YELLOW "           #####-------------------------------------- While----------------------------------------------------------- '%s', target scope '%s' -##### \n" ANSI_COLOR_RESET, instruction->itemName, scope);
    AnalyzeWhileLoopSemantics(instruction->arrayNode->elements[0], instruction->arrayNode->elements[1], instruction->arrayNode->elements[2]);
    return true;
}

void CopyCurrentInstructionStackToSymbolTable(char scope[50])
{
    printf(ANSI_COLOR_BRIGHT_YELLOW "\n#####- Copying Instruction stack to symbol table with scope '%s' -##### \n" ANSI_COLOR_RESET, scope);
    ReverseFuncInstructionStack(); 
    int counter = 1;

    while(topFuncInstruction >= 0)
    {
        
        struct Instruction* currentInstruction = GetInstructionFromStack();

        // print information about the instruction
        printf(ANSI_COLOR_GREY "\n       %d.) Copying instruction (identifier: '%s', itemKind '%s', itemType '%s', ASTNode* '%p') of type '%d' " ANSI_COLOR_RESET, 
        counter, currentInstruction->itemName, currentInstruction->itemKind, currentInstruction->itemType, currentInstruction->arrayNode->elements[0], currentInstruction->instructionType);

        CopyInstructionToSymTab(head, *currentInstruction, scope);

        PopFuncInstruction();
        counter++;
    }
    ClearFuncInstruction();
    printf(ANSI_COLOR_BRIGHT_YELLOW "\n\n#####- Finished copying '%d' instructions to the symbol table -##### \n" ANSI_COLOR_RESET, counter);

    char choice;
    printf(ANSI_COLOR_GREEN_DARK " \n Do you want to view the copied symbol table entries? (y/n): " ANSI_COLOR_RESET);
    scanf(" %c", &choice); // Note the space before %c to ignore leading whitespace


    if (choice == 'y' || choice == 'Y') 
    {
        showInstructionSymTable(GetSymbolTableFromLLScope(head,scope)->instructionTable, false);
    }             
}



void ResolveFunctionScope(char scope[50], struct ASTNode* functionRootNode) 
{
    printf(ANSI_COLOR_BRIGHT_YELLOW "\n#####- Attempting to resolve function scope '%s' -##### \n" ANSI_COLOR_RESET, scope);
    ReverseFuncInstructionStack(); 
    bool passedSemanticChecks = true;
    int counter = 1;

    while(topFuncInstruction >= 0)
    {
        
        struct Instruction* currentInstruction = GetInstructionFromStack();

        // print information about the instruction
        printf(ANSI_COLOR_BRIGHT_CYAN "\n       %d.) Resolving instruction (identifier: '%s', itemKind '%s', itemType '%s', ASTNode* '%p') of type '%d' \n" ANSI_COLOR_RESET, 
        counter, currentInstruction->itemName, currentInstruction->itemKind, currentInstruction->itemType, currentInstruction->arrayNode->elements[0], currentInstruction->instructionType);

        switch(currentInstruction->instructionType){
            case IDENTIFIER_EQ:
                passedSemanticChecks = Resolve_IDENTIFIER_EQ(currentInstruction, scope, functionRootNode);
                break;
            case NonAssignExpr_IDENTIFIER:
                passedSemanticChecks = Resolve_NonAssignExpr_IDENTIFIER(currentInstruction, scope);
                break;
            case IDENTIFIER_OPERATOR_NonAssignExpr:
                passedSemanticChecks = Resolve_IDENTIFIER_OPERATOR_NonAssignExpr(currentInstruction, scope, functionRootNode);
                break;
            case NUMBER_OPERATOR_NonAssignExpr:
                passedSemanticChecks = Resolve_NUMBER_OPERATOR_NonAssignExpr(currentInstruction, scope);
                break;
            case IDENTIFIER_LPAR_ParamList_RPAR:
                passedSemanticChecks = Resolve_IDENTIFIER_LPAR_ParamList_RPAR(currentInstruction, scope);
                break;
            case TYPE_IDENTIFIER_EQ_NonAssignExpr:
                passedSemanticChecks = Resolve_TYPE_IDENTIFIER_EQ_NonAssignExpr(currentInstruction, scope);
                break;
            case VAR_DECLARATION:
                passedSemanticChecks = localVarDecl(currentInstruction, scope);
                break;
            case PARAM_DECLARATION:
                passedSemanticChecks = ParamDecl(currentInstruction, scope);
                break;
            case WRITE_IDENTIFIER:
                passedSemanticChecks = Resolve_WRITE_IDENTIFIER(currentInstruction, scope);
                break;
            case param_IDENTIFIER:
                passedSemanticChecks = Resolve_param_IDENTIFIER(currentInstruction, scope);
                break;
            case ARRAY_IDENTIFIER_EQ:
                passedSemanticChecks = Resolve_ARRAY_IDENTIFIER_EQ(currentInstruction, scope, functionRootNode);
                break;
            case ARRAY_DECLARATION:
                passedSemanticChecks = Resolve_ARRAY_DECLARATION(currentInstruction, scope);
                break;
            case WHILEBLOCK:
                passedSemanticChecks = Resolve_WHILEBLOCK(currentInstruction, scope, functionRootNode);
                break;
            case INSTRUCTION_BREAK:
                printf(ANSI_COLOR_YELLOW "\n#####- Breaking out ResolveFunctionScope -##### \n" ANSI_COLOR_RESET);
                return;
                break;
            default:
                printf(ANSI_COLOR_BRIGHT_RED "\nERROR: Instruction type not recognized\n" ANSI_COLOR_RESET);
                break;
        }

        counter++;
        PopFuncInstruction();
       
        if(!passedSemanticChecks)
		{
			char choice;
			printf(ANSI_COLOR_BRIGHT_RED " \nSemantic error found. Do you want to try to continue to compile? (y/n): " ANSI_COLOR_RESET);
			scanf(" %c", &choice); // Note the space before %c to ignore leading whitespace

			if (choice != 'y' && choice != 'Y') 
			{
				exit(1);   
			}                   
		} 
    }
    ClearFuncInstruction();
}


#endif