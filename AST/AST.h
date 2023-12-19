#ifndef AST_H
#define AST_H

//Abstract Syntax Tree Implementation
#include <string.h>
#include <stdio.h>
#include "../Helpers/ConsoleFormating.h" // Add this line
#include "../InstructionStack/ConstantOptimizations.h"

// Define the types of nodes your AST will have
typedef enum 
{
    NODE_BINARY_EXPRESSION,
    NODE_ASSIGNMENT,
    NODE_ARRAY_ASSIGMENT,
    NODE_CONSTANT,
    NODE_VARIABLE,  // Represents both variable declarations and identifiers
    NODE_STMTLIST,
    NODE_VARIABLE_ASSIGNMENT,
    NODE_STMT,
    NODE_PROGRAM,
    NODE_BLOCKLIST,
    NODE_WRITE,
    NODE_MAIN_DECL,
    NODE_FUNC_DECL_LIST,
    NODE_FUNC_DECL,
    NODE_PARAM_DECL_LIST,
    NODE_PARAM_DECL,
    NODE_PARAM_LIST,
    NODE_PARAM,
    NODE_VAR_DECL_LIST,
    NODE_RETURN,
    NODE_ARRAY_ROOT,
    NODE_GLOB_VAR_BLOCKLIST,
    NULL_NODE,
    NODE_BLOCK,
    NODE_BLOCK_NO_RETURN,
    NODE_FUNCTION_CALL,
    NODE_BLOCK_PARSE,

    NODE_WHILE,
    NODE_CONDITIONAL_EXPRESSION,
    NODE_IF,
    NODE_BRANCH,
    NODE_BRANCHLIST,
} NodeType;

const char *NodeTypeStrings[] = {
    "NODE_BINARY_EXPRESSION",
    "NODE_ASSIGNMENT",
    "NODE_ARRAY_ASSIGMENT",
    "NODE_CONSTANT",
    "NODE_VARIABLE",
    "NODE_STMTLIST",
    "NODE_VARIABLE_ASSIGNMENT",
    "NODE_STMT",
    "NODE_PROGRAM",
    "NODE_BLOCKLIST",
    "NODE_WRITE",
    "NODE_MAIN_DECL",
    "NODE_FUNC_DECL_LIST",
    "NODE_FUNC_DECL",
    "NODE_PARAM_DECL_LIST",
    "NODE_PARAM_DECL",
    "NODE_PARAM_LIST",
    "NODE_PARAM",
    "NODE_VAR_DECL_LIST",
    "NODE_RETURN",
    "NODE_ARRAY_ROOT",
    "NODE_GLOB_VAR_BLOCKLIST",
    "NULL_NODE",
    "NODE_BLOCK",
    "NODE_BLOCK_NO_RETURN",
    "NODE_FUNCTION_CALL",
    "NODE_BLOCK_PARSE",

    "NODE_WHILE",
    "NODE_CONDITIONAL_EXPRESSION",
    "NODE_IF",
    "NODE_BRANCH",
    "NODE_BRANCHLIST",
};


// Define the possible operators for binary operations
typedef enum 
{
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
} OperatorType;




// Define a struct for array declarations
struct ArrayDeclarationNode 
{
    int size;   // Number of elements in the array
    struct ASTNode** elements; // An array to store pointers to element nodes
};



// Define the AST node struct
struct ASTNode
{
    NodeType node_type;    // a field in the struct AST that is used to represent the type or category of the node (e.g., )
    
    char *funcScope;
    char *tempVar;
    struct ASTNode* left;    // Left child node
    struct ASTNode* right;   // Right child node

    char value[50];
    char type[50];

    // Define the union
    union 
    {
        struct ArrayDeclarationNode* arrayNode;
        OperatorType operator;
        DataType dataType;
    } nodeData;

    bool isInitialized;
};

struct ASTNode* createVariableDeclarationNode(char name[50], char type[50]) 
{
    struct ASTNode* node = malloc(sizeof(struct ASTNode));      
   
    // Set the type of the node
    node->node_type = NODE_VARIABLE;

    strcpy(node->type, type);
    strcpy(node->value, name);
    
    return node;
}

struct ASTNode* createFunctionDeclarationNode(struct ASTNode* left, struct ASTNode* right, char name[50], char type[50]) 
{
    struct ASTNode* node = malloc(sizeof(struct ASTNode));      
   
    // Set the type of the node
    node->node_type = NODE_FUNC_DECL;

    strcpy(node->type, type);
    strcpy(node->value, name);

    node->left = left;
    node->right = right;
    
    return node;
}

struct ASTNode* createFunctionCallNode(struct ASTNode* left, struct ASTNode* right, char type[50], int size)
{
    struct ASTNode* node = malloc(sizeof(struct ASTNode));      
   
    // Set the type of the node
    node->node_type = NODE_FUNCTION_CALL;

    // Allocate memory for the array node to store the param values
    struct ArrayDeclarationNode* arrayNode = malloc(sizeof(struct ArrayDeclarationNode));
    arrayNode->size = size;

    // Allocate memory for the elements array
    arrayNode->elements = malloc(size * sizeof(struct ASTNode*));

    // Initialize elements to NULL (or to some default value if needed)
    for (int i = 0; i < size; i++) 
    {
        printf(ANSI_COLOR_BRIGHT_YELLOW "-- Initializing element [%d] to NULL \n  " ANSI_COLOR_RESET,i);
        arrayNode->elements[i] = NULL;
    }

    node->nodeData.arrayNode = arrayNode;

    strcpy(node->type, type);
    strcpy(node->value, "null");

    node->left = left;
    node->right = right;
    
    return node;
}

// Define a function to create an AST node for array declarations
struct ASTNode* createArrayDeclarationNode(char name[50], char type[50], int size) 
{
    struct ASTNode* node = malloc(sizeof(struct ASTNode));  
    node->node_type = NODE_ARRAY_ROOT;

    // Allocate memory for the array declaration node
    struct ArrayDeclarationNode* arrayNode = malloc(sizeof(struct ArrayDeclarationNode));

    arrayNode->size = size;

    strcpy(node->value, name);
    strcpy(node->type, type);


    // Allocate memory for the elements array
    arrayNode->elements = malloc(size * sizeof(struct ASTNode*));
    

    // Initialize elements to NULL (or to some default value if needed)
    for (int i = 0; i < size; i++) 
    {
        printf(ANSI_COLOR_BRIGHT_YELLOW "-- Initializing element [%d] to NULL \n  " ANSI_COLOR_RESET,i);
        arrayNode->elements[i] = NULL;
    }

    node->nodeData.arrayNode = arrayNode;
    return node;
}

struct ASTNode* createArrayAssignmentNode(struct ASTNode* left, struct ASTNode* right, char index[50])
{
    struct ASTNode* node = malloc(sizeof(struct ASTNode));      
   
    // Set the type of the node
    node->node_type = NODE_ARRAY_ASSIGMENT;

    strcpy(node->value, index);

    node->left = left;
    node->right = right;
    
    return node;
}


struct ASTNode* createBinaryExpressionNode(char operator[50], struct ASTNode* left, struct ASTNode* right) 
{
    struct ASTNode* node = malloc(sizeof(struct ASTNode));      
   
    // Set the type of the node
    node->node_type = NODE_BINARY_EXPRESSION;

    strcpy(node->type, operator);

    node->left = left;
    node->right = right;
    
    return node;
}


struct ASTNode* createConstantNode(char value[50], DataType type) 
{
    struct ASTNode* node = malloc(sizeof(struct ASTNode));

    node->node_type = NODE_CONSTANT;

    strcpy(node->value, value);
    node->nodeData.dataType = type;

    return node;
}


struct ASTNode* createNode(struct ASTNode* left, struct ASTNode* right, NodeType node_type) 
{
    struct ASTNode* node = malloc(sizeof(struct ASTNode));      
   
    // Set the type of the node
    node->node_type = node_type;

    node->left = left;
    node->right = right;
    
    return node;
}


void printArray(struct ASTNode* node, int level) 
{
    

    for(int i = 0; i < node->nodeData.arrayNode->size; i++) 
    {
        if (node->nodeData.arrayNode->elements[i] != NULL) 
        {
            for (int i = 0; i < level; i++)
            {
                printf("     "); 
            } 
            printf(ANSI_COLOR_CYAN" └── ""\x1b[0m");
            printf("Element: [%d]\n" , i);
            printAST(node->nodeData.arrayNode->elements[i], level + 2, 0);
            //printf("Element %d: %p\n", i, (void*)node->nodeData.arrayNode->elements[i]);
            //
        } else 
        {
            for (int i = 0; i < level; i++)
            {
                printf("     "); 
            } 
            printf(ANSI_COLOR_CYAN" └── ""\x1b[0m");
            printf("Element %d: NULL\n", i);
            //printf("Element Null");
        }
    }
}

void printNodeType(NodeType type) 
{
    printf("%s \n", NodeTypeStrings[type]);
}

void printAST(struct ASTNode* node, int level, int right)
{
    if (node == NULL)
        return;
    

    // Print indentation based on the level
    for (int i = 0; i < level-1; i++)
        printf("     ");  
    if (right == 1)
        printf(ANSI_COLOR_CYAN" └── ""\x1b[0m");
    else
        printf(ANSI_COLOR_CYAN" ├── ""\x1b[0m");
        
    switch (node->node_type){
        case NODE_VARIABLE:
            printf(ANSI_COLOR_BRIGHT_MAGENTA "Variable: " ANSI_COLOR_RESET);
            printf(ANSI_COLOR_BRIGHT_CYAN "%s (%s)\n" ANSI_COLOR_RESET, node->value, node->type);
            break;
        case NODE_VARIABLE_ASSIGNMENT:
            printf(ANSI_COLOR_BRIGHT_MAGENTA "Variable: " ANSI_COLOR_RESET);
            printf(ANSI_COLOR_BRIGHT_CYAN "%s \n" ANSI_COLOR_RESET, node->value);
            break;
        case NODE_BINARY_EXPRESSION:
            printf(ANSI_COLOR_BRIGHT_MAGENTA "Binary Expression: " ANSI_COLOR_RESET);
            printf(ANSI_COLOR_BRIGHT_CYAN "%s [evaluates to: '%s']\n" ANSI_COLOR_RESET, node->type, node->value);
            break;
        case NODE_CONSTANT:
            printf(ANSI_COLOR_BRIGHT_MAGENTA "Constant: " ANSI_COLOR_RESET);
            printf(ANSI_COLOR_BRIGHT_CYAN "%s\n" ANSI_COLOR_RESET, node->value);
            break;
        case NODE_ASSIGNMENT:
            printf(ANSI_COLOR_BRIGHT_MAGENTA "Assignment: " ANSI_COLOR_RESET);
            printf(ANSI_COLOR_BRIGHT_CYAN "= \n" ANSI_COLOR_RESET);
            break;
        case NODE_PARAM_DECL_LIST:
            printf(ANSI_COLOR_BRIGHT_YELLOW "Parameter Decl List: [temp var '%s'] \n" ANSI_COLOR_RESET, node->left->left->tempVar);
            break;
        case NODE_PARAM:
            printf(ANSI_COLOR_BRIGHT_MAGENTA "Parameter: " ANSI_COLOR_RESET);
            printf(ANSI_COLOR_BRIGHT_CYAN "%s (%s)\n" ANSI_COLOR_RESET, node->value, node->type);
            break;
        case NODE_ARRAY_ROOT:
            printf(ANSI_COLOR_BRIGHT_YELLOW "Array Declaration: \n" ANSI_COLOR_RESET);
            printArray(node, level);
            break;
        case NULL_NODE:
            break;  
        case NODE_CONDITIONAL_EXPRESSION:
            printf(ANSI_COLOR_BRIGHT_YELLOW "Conditional Expression: [ type: '%s'] \n" ANSI_COLOR_RESET, node->type);
            break;  
        case NODE_FUNCTION_CALL:
            printf(ANSI_COLOR_ORANGE "Function Call: \n" ANSI_COLOR_RESET);
            printArray(node, level);
            break;  
        case NODE_BLOCK_PARSE:
            printf(ANSI_COLOR_ORANGE "NODE BLOCK PARSE: \n" ANSI_COLOR_RESET);
            return;
            break;
        case NODE_BRANCH:
            printf(ANSI_COLOR_YELLOW "Branch: TempVar -> [%s]\n" ANSI_COLOR_RESET, node->tempVar);
            break;
        case NODE_IF:
            printf(ANSI_COLOR_YELLOW "If Block: TempVar -> [%s] , Value -> [%s] \n" ANSI_COLOR_RESET, node->tempVar, node->value);
            break;
        case NODE_WHILE:
            printf(ANSI_COLOR_YELLOW "While Block: TempVar -> [%s]\n" ANSI_COLOR_RESET, node->tempVar);
            break;
        default:
            printNodeType(node->node_type);
            break;
    }   


    // Recursively print left and right nodes with increased indentation level
    printAST(node->left, level + 1, 0);
    printAST(node->right, level + 1, 1);
}

struct ASTNode* GetParentOfNode(struct ASTNode* root, struct ASTNode* node) 
{
    if (root == NULL || node == NULL) {
        return NULL;
    }

    if (root->left == node || root->right == node) {
        return root;
    }

    struct ASTNode* leftParent = GetParentOfNode(root->left, node);
    if (leftParent != NULL) {
        return leftParent;
    }

    return GetParentOfNode(root->right, node);
}

void DeleteASTNode(struct ASTNode* root, struct ASTNode* nodeToDelete)
{
    if (root == NULL || nodeToDelete == NULL) {
        return;
    }

    struct ASTNode* parent = GetParentOfNode(root, nodeToDelete);
    if (parent != NULL) {
        if (parent->left == nodeToDelete) {
            parent->left = NULL;
        } else if (parent->right == nodeToDelete) {
            parent->right = NULL;
        }
    }

    // Recursively delete nodeToDelete and its children
    DeleteASTNode(root, nodeToDelete->left);
    DeleteASTNode(root, nodeToDelete->right);

    // Free any resources specific to nodeToDelete
    if (nodeToDelete->node_type == NODE_ASSIGNMENT) 
    {
        //free(nodeToDelete->nodeData.arrayNode->elements);
    }

    // Free nodeToDelete itself
    free(nodeToDelete);
}

void DeleteEmptyNodesOfType(struct ASTNode* root, struct ASTNode* node, NodeType type, bool freeMemory)
{
    if (node == NULL) {
        return;
    }

    if (node->node_type == type && node->left == NULL && node->right == NULL)        
    {
    
        // If this is a statement node with no children, delete it
        struct ASTNode* parent = GetParentOfNode(root, node);
        if (parent != NULL) 
        {
            if (parent->left == node) 
            {
                parent->left = NULL;
            } else if (parent->right == node) 
            {
                parent->right = NULL;
            }
        }

        if(freeMemory) free(node);
    } else 
    {
        // Recursively check left and right children
        DeleteEmptyNodesOfType(root, node->left, type, freeMemory);
        DeleteEmptyNodesOfType(root, node->right, type, freeMemory);
    }
}

struct ASTNode* getNthNodeOfType(struct ASTNode* node, NodeType targetType, int* count, int targetIndex)
{
    if (node == NULL) {
        return NULL;
    }

    if (node->node_type == targetType) {
        (*count)++;
        if (*count == targetIndex) {
            return node;
        }
    }

    struct ASTNode* result = getNthNodeOfType(node->left, targetType, count, targetIndex);
    if (result != NULL) {
        return result;
    }

    return getNthNodeOfType(node->right, targetType, count, targetIndex);
}


int CountInstances(struct ASTNode* node, NodeType targetType)
{
    if (node == NULL) {
        return 0;
    }

    int count = 0;

    if (node->node_type == targetType) {
        count++;
    }

    count += CountInstances(node->left, targetType);
    count += CountInstances(node->right, targetType);

    return count;
}



void freeAST(struct ASTNode* node) 
{
    
    if (node == NULL) {
        return;
    }

    // Recursively free left and right child nodes
    freeAST(node->left);
    freeAST(node->right);
    
    node->left = NULL;
    node->right = NULL;
    free(node);
}

#endif








































