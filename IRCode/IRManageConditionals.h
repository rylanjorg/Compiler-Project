#ifndef IR_CODE_CONDITIONALS_H
#define IR_CODE_CONDITIONALS_H

    #include "../AST/AST.h" 
    #include "IRcodeHelpers.h"
    #include "IRcodeStack.h"
    #include "IRcodeMain.h"
    #include "ConditionalLabelCounter.h"
    #include "IRcodeCondStack.h"

    #include <stdio.h>
    #include <string.h>

    char BLOCK_INDENT[50] = "\t";
    char JUMP_LABEL[] = "JUMP_C ";

 
    void ManageBranchFooter(struct ASTNode* node, char functionScope[50], char prependString[50])
    {
        char BranchFooter[100];
        // Label for Continue
        sprintf(BranchFooter, "%s%s%sContinue_%s_%c:\n\n", BLOCK_INDENT, BLOCK_INDENT, JUMP_LABEL, functionScope, BRANCH_LABEL);
        pushIRCodeStack_Conditionals(BranchFooter);
    }

    void ManageBranchHeader(struct ASTNode* node, struct ASTNode* rootNode, char prependString[50])
    {
        char BranchHeader[100];
        

        if(strcmp(node->type, "IF") == 0)
        {
            struct ASTNode* parent = GetParentOfNode(rootNode, node);
            if(parent == NULL)
            {
                printf("Error: Parent of node is NULL\n");
                exit(1);
            } 
            
            sprintf(BranchHeader, "\n%s%sIf %s %s %s %s%s", BLOCK_INDENT, prependString, SearchLeftSubTree(parent->left->left),  parent->left->type, SearchLeftSubTree(parent->left->right), JUMP_LABEL, node->tempVar);
            pushIRCodeStack(BranchHeader);
        }
        else if(strcmp(node->type, "ELSEIF") == 0)
        {
            sprintf(BranchHeader, "\n%s%sIf %s %s %s %s %s", BLOCK_INDENT, prependString, SearchLeftSubTree(node->left->left),  node->left->type, SearchLeftSubTree(node->left->right), JUMP_LABEL, node->tempVar);
            pushIRCodeStack(BranchHeader);
        }
        else if(strcmp(node->type, "ELSE") == 0)
        {
            sprintf(BranchHeader, "\n%s%s%s %s", BLOCK_INDENT, prependString, JUMP_LABEL, node->tempVar);
            pushIRCodeStack(BranchHeader);
        }
    }
    
    void ManageWhileStartHeader(struct ASTNode* node, struct ASTNode* rootNode, char functionScope[50], char prependString[50])
    {
        char BranchHeader[50];
        sprintf(BranchHeader, "%s_%s_%s", "WHILESTART", functionScope, generateNewBranchLabel(0)); 
        //pushIRCodeStack_Conditionals(BranchHeader);

        // Free existing memory if node->tempVar is not NULL
        if (node->tempVar != NULL) {
            free(node->tempVar);
        }

        // Allocate memory for node->tempVar and copy the contents of BranchHeader
        node->tempVar = strdup(BranchHeader);
    }
    
    
    void ManageWhileFooter(struct ASTNode* node, struct ASTNode* rootNode, char functionScope[50], char prependString[50])
    {
        char WhileFooter[100];
        // Label for Continue
        sprintf(WhileFooter, "%s%s%s%s:\n", BLOCK_INDENT, BLOCK_INDENT, JUMP_LABEL, rootNode->tempVar, BRANCH_LABEL);
        pushIRCodeStack_Conditionals(WhileFooter);
    }

    void ManageBranchNode(struct ASTNode* node, struct ASTNode* rootNode, char functionScope[50], char prependString[50])
    {
        char BranchHeader[50];
        sprintf(BranchHeader, "%s%s_%s_%s:\n", BLOCK_INDENT, node->type, functionScope, generateNewBranchLabel(0)); 
        pushIRCodeStack_Conditionals(BranchHeader);
        
        // Block Prepend String
        char prependConcat[100];
        strcpy(prependConcat, prependString);  
        strcat(prependConcat, BLOCK_INDENT);

        PrintConditionalBlock(node->left, node->left, functionScope, false, prependConcat);

        sprintf(BranchHeader, "%s_%s_%s:", node->type, functionScope, lastBranchLabel());

        // Free existing memory if node->tempVar is not NULL
        if (node->tempVar != NULL) {
            free(node->tempVar);
        }

        // Allocate memory for node->tempVar and copy the contents of BranchHeader
        node->tempVar = strdup(BranchHeader);

        ManageBranchHeader(node, rootNode, prependString);
    }

    void ManageWhileBlock(struct ASTNode *node, struct ASTNode *rootNode,  char functionScope[50], char prependString[50])
    {
        char WhileHeader[100];

        // Conditional Expression
        PrintWhileSubTree(node, node, functionScope, prependString);

        // Goto WhileStart:
        sprintf(WhileHeader, "%s%s%s%s:\n\n", BLOCK_INDENT, prependString, JUMP_LABEL, node->tempVar);
        pushIRCodeStack(WhileHeader);

        // WhileStart: Goto WhileBlock
        sprintf(WhileHeader, "%s%s%s:\n",BLOCK_INDENT, prependString, node->tempVar);
        pushIRCodeStack(WhileHeader);
        sprintf(WhileHeader,"%s%sIf [%s,%s,%s] %s%s\n", BLOCK_INDENT, prependString, node->left->type, SearchLeftSubTree(node->left->left), SearchLeftSubTree(node->left->right), JUMP_LABEL, node->right->tempVar);
        pushIRCodeStack(WhileHeader);

        
        // Goto Continue:
        sprintf(WhileHeader, "%s%s%sContinue_%s_%c:\n\n", BLOCK_INDENT, prependString, JUMP_LABEL,functionScope, BRANCH_LABEL);
        pushIRCodeStack(WhileHeader);

        printIRCodeStackContentsToMainStack();

        // Goto WhileStart
        //sprintf(WhileHeader, "%s%s%sGoto %s: \n", BLOCK_INDENT, BLOCK_INDENT, prependString, node->tempVar);
        //pushIRCodeStack(WhileHeader);

        pushIRCodeStack("\n");

        // Label for Continue
        sprintf(WhileHeader, "%s%sContinue_%s_%c:\n\n", BLOCK_INDENT, prependString, functionScope, BRANCH_LABEL);
        pushIRCodeStack(WhileHeader);
    }

    void ManageIfBlock(struct ASTNode *node, struct ASTNode *rootNode, char functionScope[50], char prependString[50])
    {
        char IfHeader[100];

        // Conditional Expression

        // If Sub Tree
        PrintIfSubTree(node, rootNode, functionScope, prependString);
        printAST(node,3,3);

        pushIRCodeStack("\n\n");

        printIRCodeStackContentsToMainStack();

        // Label for Continue
        sprintf(IfHeader, "%s%sContinue_%s_%c:\n\n", BLOCK_INDENT, prependString,functionScope, BRANCH_LABEL);
        pushIRCodeStack(IfHeader);

        IncrementBranchLabel();
    }


    
    void PrintIfSubTree(struct ASTNode* node, struct ASTNode* rootNode, char functionScope[50], char prependString[50])
    {
        if (node == NULL) return;

        #if PRINT_RECURS == 1
            // Add printf statements here to track the flow
            printf("[If Sub Tree] Visiting node of type: \n");
            printNodeType(node->node_type);
        #endif

        switch (node->node_type)
        {
            case NODE_BRANCH:
                ManageBranchNode(node, rootNode, functionScope, prependString);
                PrintIfSubTree(node->left, rootNode, functionScope, prependString);
                PrintIfSubTree(node->right, rootNode, functionScope, prependString);
                break;
            case NODE_BLOCK_NO_RETURN:
                printf(ANSI_COLOR_BRIGHT_MAGENTA "\n Found Conditional Block Traversing Block: \n" ANSI_COLOR_RESET);
                //PrintConditionalBlock(node->left, node->left, functionScope, false, BLOCK_INDENT);
                ManageBranchFooter(node, functionScope, prependString);
                break;
            default:
                PrintIfSubTree(node->left, rootNode, functionScope, prependString);
                PrintIfSubTree(node->right, rootNode, functionScope, prependString);
                break;
        }
    }

    void PrintWhileSubTree(struct ASTNode* node, struct ASTNode* rootNode, char functionScope[50], char prependString[50])
    {
        if (node == NULL) return;
        

        #if PRINT_RECURS == 1
            // Add printf statements here to track the flow
            printf("[While Sub Tree] Visiting node of type: \n");
            printNodeType(node->node_type);
        #endif

        switch (node->node_type)
        {
            case NODE_WHILE:
                ManageWhileStartHeader(node, rootNode, functionScope, prependString);
                PrintWhileSubTree(node->left, rootNode, functionScope, prependString);
                PrintWhileSubTree(node->right, rootNode, functionScope, prependString);
                break;
            case NODE_BRANCH:
                ManageBranchNode(node, rootNode, functionScope, prependString);
                PrintWhileSubTree(node->left, rootNode, functionScope, prependString);
                PrintWhileSubTree(node->right, rootNode, functionScope, prependString);
                break;
            case NODE_BLOCK_NO_RETURN:
                printf(ANSI_COLOR_BRIGHT_MAGENTA "\n Found Conditional Block Traversing Block: \n" ANSI_COLOR_RESET);
                //PrintConditionalBlock(node->left, node->left, functionScope, false, BLOCK_INDENT);
                ManageWhileFooter(node, rootNode, functionScope, prependString);
                break;
            default:
                PrintWhileSubTree(node->left, rootNode, functionScope, prependString);
                PrintWhileSubTree(node->right, rootNode, functionScope, prependString);
                break;
        }
    }

    void PrintConditionalBlock(struct ASTNode* root, struct ASTNode *node, char functionScope[50], bool traversedWrite, char prependString[50]) 
    {
        if (node == NULL)
        {
            //traverse = true;
            printf(ANSI_COLOR_BRIGHT_MAGENTA "[Conditional Block] null node exit: \n" ANSI_COLOR_RESET);
            return;
        }

        #if PRINT_RECURS == 1
            // Add printf statements here to track the flow
            printf(ANSI_COLOR_ORANGE"[Conditional Block] Visiting node of type: \n" );
            printNodeType(node->node_type);
            printf(ANSI_COLOR_RESET);
        #endif


        switch (node->node_type)
        {
            case NODE_ASSIGNMENT:
                ManageAssignmentNode(node, prependString, 1);
                printf(ANSI_COLOR_BRIGHT_MAGENTA "\n Found Assignment Node setting swapTreeTraversal = true" ANSI_COLOR_RESET);
                printf(ANSI_COLOR_BRIGHT_MAGENTA "\n Traversing right to left" ANSI_COLOR_RESET);
                PrintConditionalBlock(root, node->left, functionScope, traversedWrite, prependString);
                PrintConditionalBlock(root, node->right, functionScope, traversedWrite, prependString);
                //swapTreeTraversal = true;
                //swapRoot = node;
                break;
            case NODE_BINARY_EXPRESSION:
                if (!traversedWrite) {
                    PrintConditionalBlock(root, node->right, functionScope, traversedWrite, prependString);
                    PrintConditionalBlock(root, node->left, functionScope, traversedWrite, prependString);
                } else {
                    PrintConditionalBlock(root, node->left, functionScope, traversedWrite, prependString);
                    PrintConditionalBlock(root, node->right, functionScope, traversedWrite, prependString);
                }
                ManageBinaryExpressionNode(node, prependString, 1);
                break;
            case NODE_WRITE:
                PrintConditionalBlock(root, node->left, functionScope, traversedWrite, prependString);
                PrintConditionalBlock(root, node->right, functionScope, traversedWrite, prependString);
                if (!traversedWrite)
                {
                    traversedWrite = true;
                    ManageWriteNode(node, functionScope, prependString, 1);
                }
                break;
            case NODE_FUNCTION_CALL:
                ManageFunctionCall(node, prependString, 1);
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
                PrintConditionalBlock(root, node->left, functionScope, traversedWrite, prependString);
                PrintConditionalBlock(root, node->right, functionScope, traversedWrite, prependString);
                break;
        }
    }


   

#endif
