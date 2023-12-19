
#ifndef PRINT_RECURS
#define PRINT_RECURS 0
#endif


#ifndef IR_CODE_MAIN_H
#define IR_CODE_MAIN_H
// ---- Functions to handle IR code emissions ---- //

#include "../AST/AST.h" 
#include "../Helpers/ConsoleFormating.h"
#include "../MIPS/MIPScode.h"


#include "../SymbolTableManagement/SymbolTableManager.h"

#include "IRcodeHelpers.h"
#include "IRManageConditionals.h"
#include "IRcodeStack.h"

// include bool
#include <stdbool.h>

char indentation1[50] = "           ";

struct IRCodeInstructionElement {
    int intValue;
    bool isParam;
};



const char* OperatorStrings[] = 
{
   "+",
   "-",
   "*",
   "-"
};

int tempVarCounter = 0;

char* generateNewTempVar() 
{
    char *tempVar = (char*) malloc(10);
    snprintf(tempVar, 10, "t%d", ++tempVarCounter);
    return tempVar;
}

int tempVarCounterParam = 0;

char* generateNewTempVarForParam() 
{
    char *tempVar = (char*) malloc(10);
    snprintf(tempVar, 10, "p%d", ++tempVarCounterParam);
    return tempVar;
}


void ManageBinaryExpressionNode(struct ASTNode *node, char prependString[50], int writeToConditionalStack)
{
    char *newTemp = generateNewTempVar();
    char currentIRLine[100], currentMIPSLine[100];

   

    if(node->right->node_type == NODE_ASSIGNMENT)
    {
        sprintf(currentIRLine, "    %s%s = %s %s %s\n", 
        prependString,
        newTemp,
        node->left->left->tempVar ? node->left->left->tempVar :  node->left->value, // Replace "..." with actual value or var name
        node->type,
        node->right->left->tempVar ? node->right->left->tempVar : node->right->right->value // Replace "..." with actual value or var name
        );
    }
    else
    {
        printf(ANSI_COLOR_BRIGHT_MAGENTA "\n Right value: %s, Left value: %s, main node value: %s. \n"ANSI_COLOR_RESET, node->right->value, node->left->value, node->value);
        sprintf(currentIRLine, "    %s%s = %s %s %s\n", 
        prependString,
        newTemp,
        node->left->left->tempVar ? node->left->left->tempVar :  node->left->value, // Replace "..." with actual value or var name
        node->type,
        node->right->tempVar ? node->right->tempVar : node->right->right->value // Replace "..." with actual value or var name
        );
    }

    writeToConditionalStack == 1 ? pushIRCodeStack_Conditionals(currentIRLine) : pushIRCodeStack(currentIRLine);
  
    //printf(currentIRLine);

    /*sprintf(currentMIPSLine, "li $%s, %s \n", 
    newTemp,
    node->right->tempVar ? node->right->tempVar : node->right->value // Replace "..." with actual value or var name
    );
    printf(currentMIPSLine);*/

    node->tempVar = newTemp;
}

void ManageAssignmentNode(struct ASTNode *node, char prependString[50], int writeToConditionalStack)
{
    // This is a ternary conditional expression (also known as the conditional operator ?:).
    // It checks if node->left->tempVar is non-null (i.e., it has a value). If true, it returns node->left->tempVar, otherwise, it returns "..." (three dots).

    printf(ANSI_COLOR_YELLOW "\n Assignment Node \n" ANSI_COLOR_RESET);
    char currentIRLine[50], currentMIPSLine[50];


    if(IsVarDead(head, node->left) == 0)
    {
        char *newTemp = generateNewTempVar();
        printf("New Temp Var for Assignment statment: %s\n", newTemp);
        
        
        if (strcmp(node->right->type, "char") == 0){
            sprintf(currentIRLine, "    %s%s = '%s' \n", prependString, newTemp, node->right->tempVar ? node->right->tempVar : node->right->value );
        }
        else{
            sprintf(currentIRLine, "    %s%s = %s \n", prependString, newTemp, node->right->tempVar ? node->right->tempVar : node->right->value );
        }
        
        writeToConditionalStack == 1 ? pushIRCodeStack_Conditionals(currentIRLine) : pushIRCodeStack(currentIRLine);
        printf(currentIRLine);

        sprintf(currentMIPSLine, "%sli $%s, %s \n", prependString, newTemp, node->right->tempVar ? node->right->tempVar : node->right->value);
        pushMIPS(currentMIPSLine);
        node->left->tempVar = newTemp;
    }

 
}

void ManageWriteNode(struct ASTNode *node, char functionScope[50], char prependString[50], int writeToConditionalStack)
{   
    printf(ANSI_COLOR_BRIGHT_YELLOW "\n#####- Write Node AST -##### \n\n" ANSI_COLOR_RESET);
    printAST(node,2,0);

    char currentIRLine[50], currentMIPSLine[100];

    // If the left node is nodt dead, which it shouldn't be and it doesn't have a tempVar already, then create a new temp var for it
    //if(IsVarDead(head, node->left) == 0 && node->left->tempVar == NULL)
    if(IsVarDead(head, node->left) == 0)
    {
        char *newTemp = generateNewTempVar();
        printf("\nNew Temp Var for IDENTIFIER '%s': '%s'\n", node->left->value, newTemp);
        node->left->tempVar = newTemp;
        printAST(node,2,0);

        if( node->right->node_type == NODE_FUNCTION_CALL)
        {
            sprintf(currentIRLine, "    %s%s = %s \n", prependString, newTemp, node->right->tempVar ? node->right->left->tempVar : node->right->right->value );
        }
        else
        {
            sprintf(currentIRLine, "    %s%s = %s \n", prependString, newTemp, node->right->tempVar ? node->right->tempVar : node->right->value );
        }
        
        writeToConditionalStack == 1 ? pushIRCodeStack_Conditionals(currentIRLine) : pushIRCodeStack(currentIRLine);
        printf(currentIRLine);
    }

    
       
    
  


    if(IsIDENTIFIERParam(head, node->left->value, functionScope) == 1 && node->left->tempVar == NULL)
    {


        //Get the param temp var from the node
        char *paramTempVar = generateNewTempVar();
        sprintf(currentIRLine, "    print = %s (%s)\n", paramTempVar, node->left->type);
        node->left->tempVar = paramTempVar;
    }
    else
    {
        //sprintf(currentIRLine, "    %s = %s \n", node->left->tempVar, node->right->tempVar ? node->right->tempVar : node->right->value );
        //pushIRCodeStack(currentIRLine);
        if (node->left->node_type == NODE_ASSIGNMENT){
            printf("    print = %s (%s)\n", node->left->tempVar ? node->left->tempVar : node->right->value, node->left->left->type );
            sprintf(currentIRLine, "    %sprint = %s (%s)\n", prependString, node->left->tempVar ? node->left->tempVar : node->right->value, node->left->left->type );
        }
        else{
            printf("    print = %s (%s)\n", node->left->tempVar ? node->left->tempVar : node->right->value, node->left->tempVar ? node->left->type : node->right->type );
            sprintf(currentIRLine, "    %sprint = %s (%s)\n", prependString, node->left->tempVar ? node->left->tempVar : node->right->value, node->left->tempVar ? node->left->type : node->right->type );
        }
        
        
        //printf(ANSI_COLOR_BRIGHT_RED "\nStart---------------------------------------------------\n"ANSI_COLOR_RESET);
        //printAST(node,5,5);
        //printf(ANSI_COLOR_BRIGHT_RED "\nEnd---------------------------------------------------\n"ANSI_COLOR_RESET);
        //printf("    print = %s (%s)\n", node->left->tempVar ? node->left->tempVar : node->right->value, node->left->tempVar ? node->left->type : node->right->type );
        //sprintf(currentIRLine, "    print = %s (%s)\n", node->left->tempVar ? node->left->tempVar : node->right->value, node->left->tempVar ? node->left->type : node->right->type );
    }

  

   


    writeToConditionalStack == 1 ? pushIRCodeStack_Conditionals(currentIRLine) : pushIRCodeStack(currentIRLine);
    //printf(currentIRLine);
    
    sprintf(currentMIPSLine, "%smove $a0, $%s \n", prependString, node->right->tempVar ? node->right->tempVar : node->right->value);

    pushMIPS(currentMIPSLine);
    pushMIPS("li $v0, 1 \n");
    pushMIPS("syscall \n");

    
}

void ManageFuncDeclNode(struct ASTNode *node, char prependString[50], int writeToConditionalStack)
{

    char currentIRLine[300], currentMIPSLine[300];

    // Assuming your function declaration node structure includes the function name, parameters, and the function block
    char functionName[50];
    strcpy(functionName, node->right->value); // Assuming node->right represents the function name
    char parameterList[100] = "";

    struct ASTNode *params = node->right->left; // Assuming node->right->left represents the parameter list
    while (params != NULL) {
        strcat(parameterList, params->value);
        if (params->right != NULL) {
            strcat(parameterList, ", ");
        }
        params = params->right;
    }

    // Generate IR code for the function signature
    sprintf(currentIRLine, "%sFUNC %s(%s)\n", prependString, functionName, parameterList);
    writeToConditionalStack == 1 ? pushIRCodeStack_Conditionals(currentIRLine) : pushIRCodeStack(currentIRLine);
    //printf(currentIRLine);

    sprintf(currentMIPSLine, "%s:\n", functionName);
    pushMIPS(currentMIPSLine);
}

void ManageMainDeclNode(struct ASTNode *node)
{
    char FunctionHeader[100];
    sprintf(FunctionHeader, "\n%s:\n", "main");
    pushIRCodeStack(FunctionHeader);
}

void ManageFunctionCall(struct ASTNode *node, char prependString[50], int writeToConditionalStack)
{
    printf(ANSI_COLOR_BRIGHT_YELLOW "\n#####- Func Call '%s' Node AST -##### \n\n" ANSI_COLOR_RESET, node->type);

    char ParamSet[100];
    sprintf(ParamSet, "    %s#ParamSet %s \n", prependString, node->type);
    writeToConditionalStack == 1 ? pushIRCodeStack_Conditionals(ParamSet) : pushIRCodeStack(ParamSet);

    printAST(node,0,0);

    for(int i = 0; i < node->nodeData.arrayNode->size; i++)
    {
        if(node->nodeData.arrayNode->elements[i]->left->tempVar == NULL)
        {
            char *newParamTemp = generateNewTempVar();
            node->nodeData.arrayNode->elements[i]->left->tempVar = newParamTemp;
        }
        
        char assignInstruction[50];

        sprintf(assignInstruction, "    %s#%s %s = %d \n", prependString, node->nodeData.arrayNode->elements[i]->left->value, node->nodeData.arrayNode->elements[i]->left->tempVar, atoi(node->nodeData.arrayNode->elements[i]->right->value));
        writeToConditionalStack == 1 ? pushIRCodeStack_Conditionals(assignInstruction) : pushIRCodeStack(assignInstruction);
    }


    char FunctionCall[100];
    sprintf(FunctionCall, "    %sJump %s \n", prependString, node->type); 
    writeToConditionalStack == 1 ? pushIRCodeStack_Conditionals(FunctionCall) : pushIRCodeStack(FunctionCall);
}













void printIRCode(struct ASTNode* root, struct ASTNode *node, char functionScope[50], bool traversedWrite, char prependString[50]) 
{
    if (node == NULL)
    {
        //traverse = true;
        return;
    }

    #if PRINT_RECURS == 1
        // Add printf statements here to track the flow
        printf("Visiting node of type: \n");
        printNodeType(node->node_type);
    #endif

  
    switch (node->node_type)
    {
        case NODE_ASSIGNMENT:
            //ManageAssignmentNode(root, node, tempVarCounter);
            //printf(ANSI_COLOR_BRIGHT_MAGENTA "\n Found Assignment Node setting swapTreeTraversal = true" ANSI_COLOR_RESET);
            //printf(ANSI_COLOR_BRIGHT_MAGENTA "\n Traversing right to left" ANSI_COLOR_RESET);
            //swapTreeTraversal = true;
            //swapRoot = node;
            break;
        case NODE_BINARY_EXPRESSION:
            if (!traversedWrite) {
                printIRCode(root, node->right, functionScope, traversedWrite, prependString);
                printIRCode(root, node->left, functionScope, traversedWrite, prependString);
            } else {
                printIRCode(root, node->left, functionScope, traversedWrite, prependString);
                printIRCode(root, node->right, functionScope, traversedWrite, prependString);
            }
            ManageBinaryExpressionNode(node, prependString, 0);
            break;
        case NODE_WRITE:
            printIRCode(root, node->left, functionScope, traversedWrite, prependString);
            printIRCode(root, node->right, functionScope, traversedWrite, prependString);
            if (!traversedWrite)
            {
                traversedWrite = true;
                ManageWriteNode(node, functionScope, prependString, 0);
            }
            break;
        case NODE_MAIN_DECL:
            ManageMainDeclNode(node);
            printIRCode(root, node->left, functionScope, traversedWrite, prependString);
            printIRCode(root, node->right, functionScope, traversedWrite, prependString);
            break;
        case NODE_FUNCTION_CALL:
            ManageFunctionCall(node, prependString, 0);
            break;
        case NODE_BLOCK_PARSE:
            return;
            break;
        case NODE_WHILE:
            ManageWhileBlock(node, root, functionScope, prependString);
            break;
         case NODE_IF:
            ManageIfBlock(node, root, functionScope, prependString);
            break;
        default:
            printIRCode(root, node->left, functionScope, traversedWrite, prependString);
            printIRCode(root, node->right, functionScope, traversedWrite, prependString);
            break;
    }
}



void writeFunctionIRCodeToFile(struct ASTNode* functionRoot, char functionScope[50])
{
    //char* leftKind = GetIDENTIFIERKind(head, instruction->itemName, scope, true);
    //if(strcmp(leftKind, "Param") == 0) { printf(ANSI_COLOR_BRIGHT_MAGENTA "\n%s--> Resolve: Constant folding not performed because IDENTIFIER '%s' is a param type " ANSI_COLOR_RESET, "           ", instruction->itemName); return true; }




    //int tempVarCounter = 0;
    FILE *file;

    // appead to the current file
    file = fopen("OutputFiles/IRCode.ir", "a"); 
    
    if (file == NULL) {
        printf("Error opening IR.ir\n");
        return;
    }

    char FunctionHeader[100];
    printf("\n%s%s:\n", indentation1, functionScope);
    sprintf(FunctionHeader, "\n%s:\n", functionScope);

    pushIRCodeStack(FunctionHeader);

    // generate IR code for the function
    printIRCode(functionRoot->right,  functionRoot->right, functionScope, false, "");

    char returnInstruction[50];
    printf("%sReturn To Caller\n", indentation1);
    sprintf(returnInstruction, "    Return To Caller\n");
    pushIRCodeStack(returnInstruction);
    
    pushIRCodeStack("\n");

    // print the IR code stack contents to the file
    printIRCodeStackContentsToFile(file); 

    // close the file
    fclose(file);
}




void WriteIRCodeToFile(struct ASTNode *node, struct SymbolTableNode* globalSymbolTable)
{
    //int tempVarCounter = 0;
    FILE *file,*mips_file;


    // "w" means open for writing (creates if not exist)
    file = fopen("OutputFiles/IRCode.ir", "a"); 
    mips_file = fopen("OutputFiles/MipsScript.s", "w");
    
    if (file == NULL || mips_file == NULL ) {
        printf("Error opening IR.ir or MIPS.s file.\n");
        return;
    }

    clearIRCodeStack();
    clearMIPS();

    //getNthNodeOfType(struct ASTNode* node, NodeType targetType, int* count, int targetIndex)
    int count = 0;
    struct ASTNode* mainRoot = getNthNodeOfType(node, NODE_MAIN_DECL, &count, 1);
    if(mainRoot == NULL) {printf(ANSI_COLOR_BRIGHT_RED "No Main Function Found\n" ANSI_COLOR_RESET); return;}

    printIRCode(mainRoot,  mainRoot, "main", false, "");

    printIRCodeStackContentsToFile(file); 
    fclose(file);

    printMIPSStackContentsToFile(mips_file);
    fclose(mips_file);
}



#endif
