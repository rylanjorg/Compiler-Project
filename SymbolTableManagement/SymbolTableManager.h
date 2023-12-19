


#ifndef SYMBOL_TABLE_MANAGER_H
#define SYMBOL_TABLE_MANAGER_H

    #include "GlobalSymbolTable.h"
    #include "../Helpers/Dictionary.h"
    #include "../IRCode/IRcodeMain.h"
    #include "../AST/AST.h"
    
    char indentation2[50] = "           ";
    
    typedef enum 
    {
        VAR_DECLARATION,
        PARAM_DECLARATION,
        TYPE_IDENTIFIER_EQ_NonAssignExpr,
        IDENTIFIER_EQ,
        NonAssignExpr_IDENTIFIER,
        IDENTIFIER_OPERATOR_NonAssignExpr,
        IDENTIFIER_LPAR_ParamList_RPAR,
        NUMBER_OPERATOR_NonAssignExpr,
        WRITE_IDENTIFIER,
        param_IDENTIFIER,
        INSTRUCTION_BREAK,
        ARRAY_IDENTIFIER_EQ,
        ARRAY_DECLARATION,
        WHILEBLOCK,
        
    } InstructionType;


    struct Instruction 
    {
        /// summary: 
        /// Add item to function symbol table instruction.
        /// addItemToFuncSymTab(struct SymbolTableNode* funcSymTabNode, char itemName[50], char itemKind[8], char itemType[8], struct ASTNode* ast_node_ref)
        char itemName[50];
        char itemKind[8];
        char itemType[8];

        struct ArrayDeclarationNode* arrayNode;

        struct ASTNode* assignmentNodeRef;

        InstructionType instructionType;
    };


    int counter = 0;

    typedef enum 
    {
        GlobalTable,
        FunctionTable,
        InstructionTable,
        ConditionalBlockTable,
    } SymbolTableType;


    struct GlobalTableEntry
    {
        int itemID;
        int numRef;
        int is_dead;
        char itemName[50];  //the name of the identifier
        char itemKind[8];  //is it a function or a variable?
        char itemType[8];  // Is it int, char, etc.?
        int arrayLength;
        struct ASTNode* ast_node_ref;
        struct ASTNode* assignment_node_ref;
        char itemValue[50];
    };

    struct ConditionalBlockTableEntry
    {
        int itemID;
        int numRef;
        int is_dead;
        char itemName[50];  //the name of the identifier
        char itemKind[8];  //is it a function or a variable?
        char itemType[8];  // Is it int, char, etc.?
        int arrayLength;
        struct ASTNode* ast_node_ref;
        struct ASTNode* assignment_node_ref;
        char itemValue[50];
    };

    struct FunctionTableEntry
    {
        int itemID;
        int numRef;
        int is_dead;
        char itemName[50];  //the name of the identifier
        char itemKind[8];  //is it a function or a variable?
        char itemType[8];  // Is it int, char, etc.?
        int arrayLength;
        struct ASTNode* ast_node_ref;
        struct ASTNode* assignment_node_ref;
        char itemValue[50];
    };

    // Define the symbol table node struct
    struct SymbolTableNode 
    {
        char symTabScope[50];
        int symTabIndex;
        int symTabEntryCounter;
        bool isInitialized;
        SymbolTableType tableType;
        
        
        union 
        {
            struct GlobalTableEntry** globalSymTab;
            struct FunctionTableEntry** funcSymTab;
            struct Instruction** instructionSymTab;
        } symTableEntryType;

        struct SymbolTableNode* next;
        struct SymbolTableNode* prev;  // Added field for previous node
        struct SymbolTableNode* instructionTable;

        // array of pointers to ConditionalBlock Tables for if/else/ifelse/while
        int numConditionalBlocks;
        struct SymbolTableNode** localConditionalBlockTables;
    };


    int getSymTabIndex()
    {
        return counter++;
    }

    struct SymbolTableNode* GetSymbolTableFromLLScope(struct SymbolTableNode* node, char scope[50])
    {
        while (node != NULL) 
        {
            //printf(ANSI_COLOR_GREY "\n%s--> Info: GetSymbTabFromScope-> Current scope '%s', scope to find '%s'" ANSI_COLOR_RESET,"          ", node->symTabScope, scope);
            if(strcmp(node->symTabScope, scope) == 0)
            {
                return node;
            }
            else
            {
                node = node->next;
            }
        }
        printf(ANSI_COLOR_BRIGHT_RED "\n ERROR: failed to get symbol table with scope '%s' \n	REASON: -> Symbol table not found in linked list \n" ANSI_COLOR_RESET, scope);
        return NULL;
    }
       
    void showFuncSymTable(struct SymbolTableNode* symTabRef, bool showAll)
    {
        printf("itemID  numRef  isDead?  itemName    itemKind    itemType  ArrayLength    ASTNode*  ASTNode*(assign) itemValue\n");
        printf("------------------------------------------------------------------------------------------------------------------------\n");
        for (int i = 0; showAll == true? i < 100: i < symTabRef->symTabEntryCounter; i++)
        {
            printf("%5d %5d %5d %15s  %7s  %7s    %6d            %p       %p      %s\n", 
                symTabRef->symTableEntryType.funcSymTab[i]->itemID, 
                symTabRef->symTableEntryType.funcSymTab[i]->numRef, 
                symTabRef->symTableEntryType.funcSymTab[i]->is_dead, 
                symTabRef->symTableEntryType.funcSymTab[i]->itemName, 
                symTabRef->symTableEntryType.funcSymTab[i]->itemKind, 
                symTabRef->symTableEntryType.funcSymTab[i]->itemType, 
                symTabRef->symTableEntryType.funcSymTab[i]->arrayLength, 
                (void*)symTabRef->symTableEntryType.funcSymTab[i]->ast_node_ref, 
                (void*)symTabRef->symTableEntryType.funcSymTab[i]->assignment_node_ref, 
                symTabRef->symTableEntryType.funcSymTab[i]->itemValue);
        }

        printf("------------------------------------------------------------------------------------------------------------------------\n");
    }
    
    void showGlobSymTable(struct SymbolTableNode* symTabRef, bool showAll)
    {
        printf("itemID  numRef  isDead?  itemName    itemKind    itemType  ArrayLength    ASTNode*  ASTNode*(assign) itemValue\n");
        printf("------------------------------------------------------------------------------------------------------------------------\n");
        for (int i = 0; showAll == true? i < 100: i < symTabRef->symTabEntryCounter; i++)
        {
            printf("%5d %5d %5d %15s  %7s  %7s    %6d            %p       %p      %s\n", 
                symTabRef->symTableEntryType.globalSymTab[i]->itemID, 
                symTabRef->symTableEntryType.globalSymTab[i]->numRef, 
                symTabRef->symTableEntryType.globalSymTab[i]->is_dead, 
                symTabRef->symTableEntryType.globalSymTab[i]->itemName, 
                symTabRef->symTableEntryType.globalSymTab[i]->itemKind, 
                symTabRef->symTableEntryType.globalSymTab[i]->itemType, 
                symTabRef->symTableEntryType.globalSymTab[i]->arrayLength, 
                (void*)symTabRef->symTableEntryType.globalSymTab[i]->ast_node_ref, 
                (void*)symTabRef->symTableEntryType.globalSymTab[i]->assignment_node_ref, 
                symTabRef->symTableEntryType.globalSymTab[i]->itemValue);
        }

        printf("------------------------------------------------------------------------------------------------------------------------\n");
    }

    void showInstructionSymTable(struct SymbolTableNode* instructionTable, bool showAll)
    {
        printf("\n------------------------------------------------------------------------------------------------------------------------\n");
        for (int i = 0; showAll == true? i < 100: i < instructionTable->symTabEntryCounter; i++)
        {
            printf(ANSI_COLOR_GREY "\n  [%d] -> instruction (identifier: '%s', itemKind '%s', itemType '%s', ASTNode* '%p') of type '%d' " ANSI_COLOR_RESET, 
            i, 
            instructionTable->symTableEntryType.instructionSymTab[i]->itemName, 
            instructionTable->symTableEntryType.instructionSymTab[i]->itemKind, 
            instructionTable->symTableEntryType.instructionSymTab[i]->itemType, 
            instructionTable->symTableEntryType.instructionSymTab[i]->arrayNode[0], 
            instructionTable->symTableEntryType.instructionSymTab[i]->instructionType);
        }

        printf("\n------------------------------------------------------------------------------------------------------------------------\n");
    }

    void ShowAllSymTabs(struct SymbolTableNode* head)
    {
        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            if(current->tableType == GlobalTable)
            {
                printf(ANSI_COLOR_BRIGHT_YELLOW "\n #####- Global Symbol Table -##### \n" ANSI_COLOR_RESET);
                showGlobSymTable(current, false);
            }
            else if(current->tableType == FunctionTable)
            {
                printf(ANSI_COLOR_BRIGHT_YELLOW "\n #####- Function '%s' Symbol Table -##### \n" ANSI_COLOR_RESET, current->symTabScope);
                showFuncSymTable(current, false);
            }
            current = current->next; // Move to the next node
        }

        printf("\n");
    }



    struct SymbolTableNode* insertInstructionTable(int index, char scope[50], SymbolTableType tableType) 
    {

        // Step 1: Create a new node and allocate memory for it
        struct SymbolTableNode* newNode = (struct SymbolTableNode*) malloc(sizeof(struct SymbolTableNode));


        // Step 2: Fetch the next available symbol table index and set it
        newNode->symTabIndex = index;

        // Step 3: Set the symbol table entry counter to 0
        newNode->symTabEntryCounter = 0;

        // Step 4: Set the symbol table scope
        strcpy(newNode->symTabScope, scope);

        // Step 5: Set the symbol table type
        newNode->tableType = tableType;

        // Step 6: Allocate memory for 100 entries of the corresponding type
        if (tableType == InstructionTable) 
        {
            newNode->symTableEntryType.instructionSymTab = (struct Instruction**) malloc(sizeof(struct Instruction*) * 100);
            for (int i = 0; i < 100; i++) {
                newNode->symTableEntryType.instructionSymTab[i] = NULL;
                //newNode->symTableEntryType.instructionSymTab[i] = (struct Instruction*) malloc(sizeof(struct Instruction));
            }
        } 
    
        // Step 8: Set isInitalized flag to false;
        newNode->isInitialized = false;

        printf(ANSI_COLOR_BRIGHT_YELLOW "\n#####- Initializing Instruction Symbol Table -##### \n \n Creating new Symbol Table Node of type %s, \n with scope %s, at index %d, and setting entry counter to %d.\n" ANSI_COLOR_RESET,
                newNode->tableType == InstructionTable ? "InstructionTable" : "Error", newNode->symTabScope, newNode->symTabIndex, newNode->symTabEntryCounter);         

        return newNode;                   
    }


    void ResetSymbolTable(struct SymbolTableNode* head, char scope[50])
    {
        struct SymbolTableNode* functionTable = GetSymbolTableFromLLScope(head, scope);
        functionTable->symTabEntryCounter = 0;


        if (functionTable == NULL)
        {
            printf(ANSI_COLOR_BRIGHT_RED "\n ERROR: failed to reset symbol table with scope '%s' \n	REASON: -> Symbol table not found in linked list \n" ANSI_COLOR_RESET, scope);
            return;
        }

        if (functionTable->tableType != FunctionTable)
        {
            printf(ANSI_COLOR_BRIGHT_RED "\n ERROR: failed to reset symbol table with scope '%s' \n	REASON: -> Symbol table is not a function table \n" ANSI_COLOR_RESET, scope);
            return;
        }

        for (int i = 0; i < 100; i++) 
        {
            functionTable->symTableEntryType.funcSymTab[i] = NULL;
            free(functionTable->symTableEntryType.funcSymTab[i]); // Free each entry in the table
            functionTable->symTableEntryType.funcSymTab[i] = (struct FunctionTableEntry*) malloc(sizeof(struct FunctionTableEntry));
        }

        printf(ANSI_COLOR_BRIGHT_GREEN "\n Symbol table with scope '%s' was reset successfully. \n" ANSI_COLOR_RESET, scope);
    }


    char* GetIDENTIFIERType(struct SymbolTableNode* head, char itemName[50], char* scope,  bool checkGlobalScope)
    {   
        // Allocate memory for itemType
        char* resultString =  malloc(8 * sizeof(char));     

        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                
                // Inside each node, iterate through the symbol table
                for (int i = 0; i < current->symTabEntryCounter; i++) 
                {
                    if(current->tableType == GlobalTable)
                    {
                        if (strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0) 
                        {
                            strcpy(resultString, current->symTableEntryType.globalSymTab[i]->itemType);
                        }
                    }
                    else if(current->tableType == FunctionTable)
                    {
                        if (strcmp(current->symTableEntryType.funcSymTab[i]->itemName, itemName) == 0) 
                        {
                            strcpy(resultString, current->symTableEntryType.funcSymTab[i]->itemType);
                        }
                    }
                }

                if(checkGlobalScope)
                {
                    // Do an aditional check to see whether the identifier is in the global scope
                    if (strcmp(current->symTabScope, "Global") == 0) 
                    {
                        // Inside each node, iterate through the symbol table
                        for (int i = 0; i < current->symTabEntryCounter; i++) 
                        {
                            if (strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0) 
                            {
                                printf(ANSI_COLOR_YELLOW"\n--> Identifier definition found in Global Symbol table '%s'. " ANSI_COLOR_RESET, itemName);
                                strcpy(resultString, current->symTableEntryType.globalSymTab[i]->itemType);
                                strcpy(scope, "Global");
                            }
                        }
                    }
                }
            }
            current = current->next; // Move to the next node
        }

        return resultString; // Identifier not found
    }


    char* GetIDENTIFIERKind(struct SymbolTableNode* head, char itemName[50], char* scope,  bool checkGlobalScope)
    {   
        // Allocate memory for itemType
        char* resultString =  malloc(8 * sizeof(char));     

        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                
                // Inside each node, iterate through the symbol table
                for (int i = 0; i < current->symTabEntryCounter; i++) 
                {
                    if(current->tableType == GlobalTable)
                    {
                        if (strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0) 
                        {
                            strcpy(resultString, current->symTableEntryType.globalSymTab[i]->itemKind);
                        }
                    }
                    else if(current->tableType == FunctionTable)
                    {
                        if (strcmp(current->symTableEntryType.funcSymTab[i]->itemName, itemName) == 0) 
                        {
                            strcpy(resultString, current->symTableEntryType.funcSymTab[i]->itemKind);
                        }
                    }
                }

                if(checkGlobalScope)
                {
                    // Do an aditional check to see whether the identifier is in the global scope
                    if (strcmp(current->symTabScope, "Global") == 0) 
                    {
                        // Inside each node, iterate through the symbol table
                        for (int i = 0; i < current->symTabEntryCounter; i++) 
                        {
                            if (strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0) 
                            {
                                printf(ANSI_COLOR_YELLOW"\n--> Identifier definition found in Global Symbol table '%s'. " ANSI_COLOR_RESET, itemName);
                                strcpy(resultString, current->symTableEntryType.globalSymTab[i]->itemKind);
                                strcpy(scope, "Global");
                            }
                        }
                    }
                }
            }
            current = current->next; // Move to the next node
        }

        return resultString; // Identifier not found
    }


    void insertSymbolTable(struct SymbolTableNode** headRef, char scope[50], SymbolTableType tableType, int numLocalConditionalBlocks) 
    {

        // Step 1: Create a new node and allocate memory for it
        struct SymbolTableNode* newNode = (struct SymbolTableNode*) malloc(sizeof(struct SymbolTableNode));


        // Step 2: Fetch the next available symbol table index and set it
        newNode->symTabIndex = getSymTabIndex();

        // Step 3: Set the symbol table entry counter to 0
        newNode->symTabEntryCounter = 0;

        // Step 4: Set the symbol table scope
        strcpy(newNode->symTabScope, scope);

        size_t size = sizeof(newNode->symTabScope);

        // Step 5: Set the symbol table type
        newNode->tableType = tableType;

        // Step 6: Allocate memory for 100 entries of the corresponding type
        if (tableType == GlobalTable) 
        {
            newNode->symTableEntryType.globalSymTab = (struct GlobalTableEntry**) malloc(sizeof(struct GlobalTableEntry*) * 100);
            for (int i = 0; i < 100; i++) {
                newNode->symTableEntryType.globalSymTab[i] = (struct GlobalTableEntry*) malloc(sizeof(struct GlobalTableEntry));
            }
        } 
        else if (tableType == FunctionTable) 
        {
            newNode->symTableEntryType.funcSymTab = (struct FunctionTableEntry**) malloc(sizeof(struct FunctionTableEntry*) * 100);
            for (int i = 0; i < 100; i++) {
                newNode->symTableEntryType.funcSymTab[i] = (struct FunctionTableEntry*) malloc(sizeof(struct FunctionTableEntry));
            }
        }

        newNode->numConditionalBlocks = numLocalConditionalBlocks;
        // Step 7: Allocate memory for 20 conditional blocks 
        if (tableType != InstructionTable) 
        {
            newNode->localConditionalBlockTables = (struct SymbolTableNode**) malloc(sizeof(struct SymbolTableNode*) * 20);
            for (int i = 0; i < 20; i++) {
                newNode->localConditionalBlockTables[i] = NULL;
            }
        } 


       // Make the new node point to the current head
        newNode->next = (*headRef);

        // Update the head to point to the new node
        (*headRef) = newNode;


        printf(ANSI_COLOR_BRIGHT_YELLOW "\n#####- Initializing Symbol Table -##### \n \n Creating new Symbol Table Node of type %s, \n with scope %s (char ST array size: %zu), \n at index %d, and setting entry counter to %d. \n This Symbol has head->%s and prev->%s.  \n" ANSI_COLOR_RESET,
                newNode->tableType == GlobalTable ? "GlobalTable" : "FunctionTable", newNode->symTabScope, size, newNode->symTabIndex, newNode->symTabEntryCounter, newNode->next == NULL ? "NULL" : newNode->next->symTabScope, newNode->prev == NULL ? "NULL" : newNode->prev->symTabScope);    
    

        newNode->instructionTable = insertInstructionTable(newNode->symTabIndex,scope, InstructionTable);     

        printf(ANSI_COLOR_GREY "\n --> Info: Instruction Table pointer -> '%p' \n" ANSI_COLOR_RESET, newNode->instructionTable);            
    }


    /*
    struct SymbolTableNode* insertInstructionTable(int index, char scope[50], SymbolTableType tableType, int numLocalConditionalBlocks) 
    {

        // Step 1: Create a new node and allocate memory for it
        struct SymbolTableNode* newNode = (struct SymbolTableNode*) malloc(sizeof(struct SymbolTableNode));


        // Step 2: Fetch the next available symbol table index and set it
        newNode->symTabIndex = index;

        // Step 3: Set the symbol table entry counter to 0
        newNode->symTabEntryCounter = 0;

        // Step 4: Set the symbol table scope
        strcpy(newNode->symTabScope, scope);

        // Step 5: Set the symbol table type
        newNode->tableType = tableType;

        // Step 6: Allocate memory for 100 entries of the corresponding type
        if (tableType == InstructionTable) 
        {
            newNode->symTableEntryType.instructionSymTab = (struct Instruction**) malloc(sizeof(struct Instruction*) * 100);
            for (int i = 0; i < 100; i++) {
                newNode->symTableEntryType.instructionSymTab[i] = NULL;
                //newNode->symTableEntryType.instructionSymTab[i] = (struct Instruction*) malloc(sizeof(struct Instruction));
            }
        } 

        newNode->numConditionalBlocks = numLocalConditionalBlocks;
        // Step 7: Allocate memory for 20 conditional blocks 
        if (tableType != InstructionTable) 
        {
            newNode->localConditionalBlockTables = (struct SymbolTableNode**) malloc(sizeof(struct SymbolTableNode*) * 20);
            for (int i = 0; i < 20; i++) {
                newNode->localConditionalBlockTables[i] = NULL;
            }
        } 

        // Step 8: Set isInitalized flag to false;
        newNode->isInitialized = false;

        printf(ANSI_COLOR_BRIGHT_YELLOW "\n#####- Initializing Instruction Symbol Table -##### \n \n Creating new Symbol Table Node of type %s, \n with scope %s, at index %d, and setting entry counter to %d.\n" ANSI_COLOR_RESET,
                newNode->tableType == InstructionTable ? "InstructionTable" : "Error", newNode->symTabScope, newNode->symTabIndex, newNode->symTabEntryCounter);         

        return newNode;                   
    }
    */












    
    int CheckForIdentifierInSymbTab(struct SymbolTableNode* head, char itemName[50], char* scope,  bool checkGlobalScope)
    {
        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                printf(ANSI_COLOR_CYAN "--> Currently searching symbol table '%s', that has '%d' entries. \n" ANSI_COLOR_RESET, scope, current->symTabEntryCounter);
                
                // Inside each node, iterate through the symbol table
                for (int i = 0; i < current->symTabEntryCounter; i++) 
                {
                    if(current->tableType == GlobalTable)
                    {
                        if (strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0) 
                        {
                            return 1; // Identifier found
                        }
                    }
                    else if(current->tableType == FunctionTable)
                    {
                        if (strcmp(current->symTableEntryType.funcSymTab[i]->itemName, itemName) == 0) 
                        {
                            return 1; // Identifier found
                        }
                    }
                }

                if(checkGlobalScope)
                {
                    // Do an aditional check to see whether the identifier is in the global scope
                    if (strcmp(current->symTabScope, "Global") == 0) 
                    {
                        printf(ANSI_COLOR_CYAN "\n--> Searching Global Symbol table for definition of identifier '%s'. " ANSI_COLOR_RESET, itemName);
                        
                        // Inside each node, iterate through the symbol table
                        for (int i = 0; i < current->symTabEntryCounter; i++) 
                        {
                            if (strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0) 
                            {
                                printf(ANSI_COLOR_YELLOW"\n--> Identifier definition found in Global Symbol table '%s'. " ANSI_COLOR_RESET, itemName);
                                strcpy(scope, "Global");
                                return 1; // Identifier found
                            }
                        }
                    }
                }
            }
            current = current->next; // Move to the next node
        }
        return 0; // Identifier not found
    }


    int CheckForIdentifierInSymbTabIndent(struct SymbolTableNode* head, char itemName[50], char* scope, char indent[50], bool checkGlobalScope)
    {
        // Iterate through the linked list
        struct SymbolTableNode* current = head;



        while (current != NULL) 
        {
            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                printf(ANSI_COLOR_GREY "%s--> Currently searching symbol table '%s', that has '%d' entries. \n" ANSI_COLOR_RESET, indent, scope, current->symTabEntryCounter);
                
                // Inside each node, iterate through the symbol table
                for (int i = 0; i < current->symTabEntryCounter; i++) 
                {
                    if(current->tableType == GlobalTable)
                    {
                        if (strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0) 
                        {
                            return 1; // Identifier found
                        }
                    }
                    else if(current->tableType == FunctionTable)
                    {
                        if (strcmp(current->symTableEntryType.funcSymTab[i]->itemName, itemName) == 0) 
                        {
                            return 1; // Identifier found
                        }
                    }
                }
            }

            if(checkGlobalScope)
            {
                // Do an aditional check to see whether the identifier is in the global scope
                if (strcmp(current->symTabScope, "Global") == 0) 
                {
                    printf(ANSI_COLOR_GREY "%s--> Searching Global Symbol table for definition of identifier '%s'.\n" ANSI_COLOR_RESET, indent, itemName);
                    
                    // Inside each node, iterate through the symbol table
                    for (int i = 0; i < current->symTabEntryCounter; i++) 
                    {
                        if (strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0) 
                        {
                            
                            printf(ANSI_COLOR_YELLOW "%s--> Identifier definition found in Global Symbol table '%s'.\n" ANSI_COLOR_RESET, indent, itemName);
                            strcpy(scope, "Global");
                            return 1; // Identifier found
                        }
                    }
                }
            }   

            current = current->next; // Move to the next node
        }
        return 0; // Identifier not found
    }


    int GetNumParamsInFuncSymTab(struct SymbolTableNode* head, char scope[50])
    {
        int counter = 0;
        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {

            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                
                // Inside each node, iterate through the symbol table
                for (int i = 0; i < current->symTabEntryCounter; i++) 
                {

                    if(current->tableType == FunctionTable)
                    {
                        if (strcmp(current->symTableEntryType.funcSymTab[i]->itemKind, "Param") == 0) 
                        {
                            counter++;
                        }
                    }
                }
            }
            current = current->next; // Move to the next node
        }
        return counter;
    }


    char* GetParamType(struct SymbolTableNode* head, char scope[50], int index)
    {
        // Allocate memory for itemType
        char* itemType = malloc(8 * sizeof(char));

        // Check if memory allocation was successful
        if (itemType == NULL) {
            // Handle memory allocation error
            return NULL;
        }

        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                strcpy(itemType, current->symTableEntryType.funcSymTab[index]->itemType);
                return itemType;
            }

            current = current->next; // Move to the next node
        }

        // Free memory if no matching scope is found
        free(itemType);

        // Return NULL if no matching scope is found
        return NULL;
    }


    int IsIDENTIFIERParam(struct SymbolTableNode* head, char itemName[50], char scope[50])
    {
        // Get whether the identifier is a param kind
        struct SymbolTableNode* current = GetSymbolTableFromLLScope(head, scope);

         // Inside each node, iterate through the symbol table
        for (int i = 0; i < current->symTabEntryCounter; i++) 
        {
            if (strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0) 
            {
                if (strcmp(current->symTableEntryType.globalSymTab[i]->itemKind, "Param") == 0)
                {
                    return 1; 
                } 
                else 
                {
                    return 0; 
                }
            }
            else if (strcmp(current->symTableEntryType.funcSymTab[i]->itemName, itemName) == 0) 
            {
                 if (strcmp(current->symTableEntryType.funcSymTab[i]->itemKind, "Param") == 0)
                {
                    return 1; // The ast_node_ref is pointing to something
                } 
                else 
                {
                    return 0; // The ast_node_ref is NULL
                }
            }
        }

    }


    char* GetIdentifierName(struct SymbolTableNode* head, char scope[50], int index)
    {
        // Allocate memory for itemType
        char* itemName = malloc(50 * sizeof(char));

        // Check if memory allocation was successful
        if (itemName == NULL) {
            // Handle memory allocation error
            return NULL;
        }

        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                strcpy(itemName, current->symTableEntryType.funcSymTab[index]->itemName);
                return itemName;
            }

            current = current->next; // Move to the next node
        }

        // Free memory if no matching scope is found
        free(itemName);

        // Return NULL if no matching scope is found
        return NULL;
    }


    int TryUpdateVarValue(struct SymbolTableNode* head, char itemName[50], char scope[50], char value[50])
    {
        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        printf(ANSI_COLOR_GREY "\n           --> Info: Trying to set identifier '%s' in scope '%s' to value '%s'. " ANSI_COLOR_RESET,itemName, scope, value);

        while (current != NULL) 
        {
            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                // Inside each node, iterate through the symbol table
                for (int i = 0; i < current->symTabEntryCounter; i++) 
                {
                    if (current->tableType == GlobalTable) 
                    {
                        if(strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0)
                        {
                            strcpy(current->symTableEntryType.globalSymTab[i]->itemValue, value);
                            return 1; // Variable value updated
                        }  
                    }
                    else if (current->tableType == FunctionTable)
                    {
                        if(strcmp(current->symTableEntryType.funcSymTab[i]->itemName, itemName) == 0) 
                        {
                            strcpy(current->symTableEntryType.funcSymTab[i]->itemValue, value);
                            return 1; // Variable value updated
                        }       
                    }
                }
            }
            current = current->next; // Move to the next node
        }

        // If the function reaches this point, it means the identifier wasn't found
        printf(ANSI_COLOR_BRIGHT_YELLOW "\n ERROR: failed to set variable '%s' to '%s' in scope '%s' \n	REASON: -> Identifier not found in symbol table \n" ANSI_COLOR_RESET, itemName,  value, scope);
        return 0;
    }


    char* TryGetVarValue(struct SymbolTableNode* head, char itemName[50], char scope[50])
    {
        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                // Inside each node, iterate through the symbol table
                for (int i = 0; current->symTabEntryCounter; i++) 
                {
                    if(current->symTableEntryType.globalSymTab[i] != NULL &&
                    strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0)
                    {
                        return current->symTableEntryType.globalSymTab[i]->itemValue;
                    }
                    else if(current->symTableEntryType.funcSymTab[i] != NULL &&
                            strcmp(current->symTableEntryType.funcSymTab[i]->itemName, itemName) == 0)
                    {
                        return current->symTableEntryType.funcSymTab[i]->itemValue;
                    }
                }
            }
            current = current->next; // Move to the next node
        }

        return NULL; // Identifier not found
    }


    int TryUpdateAssignmentNodeRef(struct SymbolTableNode* head, char itemName[50], char scope[50], struct ASTNode* assignment_node)
    {
        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                // Inside each node, iterate through the symbol table
                for (int i = 0; current->symTabEntryCounter; i++) 
                {
                    if (current->symTableEntryType.globalSymTab[i] != NULL &&
                        strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0) 
                    {
                        current->symTableEntryType.globalSymTab[i]->assignment_node_ref = assignment_node;
                        return 1; // Assignment node reference updated
                    }
                    else if (current->symTableEntryType.funcSymTab[i] != NULL &&
                            strcmp(current->symTableEntryType.funcSymTab[i]->itemName, itemName) == 0) 
                    {
                        current->symTableEntryType.funcSymTab[i]->assignment_node_ref = assignment_node;
                        return 1; // Assignment node reference updated
                    }
                }
            }
            current = current->next; // Move to the next node
        }

        // If the function reaches this point, it means the identifier wasn't found
        printf(ANSI_COLOR_BRIGHT_YELLOW "\n ERROR: failed to set assignment node ref for variable '%s' in scope '%s' \n	REASON: -> Identifier not found in symbol table \n" ANSI_COLOR_RESET, itemName,  scope);
        return 0;
    }


    int DoesVarAlreadyHaveAssignment(struct SymbolTableNode* head, char itemName[50], char scope[50])
    {
        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                // Inside each node, iterate through the symbol table
                for (int i = 0; i < current->symTabEntryCounter; i++) 
                {
                    if (current->symTableEntryType.globalSymTab[i] != NULL &&
                        strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0) 
                    {
                        if (current->symTableEntryType.globalSymTab[i]->assignment_node_ref != NULL) 
                        {
                            return 1; // The ast_node_ref is pointing to something
                        } 
                        else 
                        {
                            return 0; // The ast_node_ref is NULL
                        }
                    }
                    else if (current->symTableEntryType.funcSymTab[i] != NULL &&
                            strcmp(current->symTableEntryType.funcSymTab[i]->itemName, itemName) == 0) 
                    {
                        if (current->symTableEntryType.funcSymTab[i]->assignment_node_ref != NULL) 
                        {
                            return 1; // The ast_node_ref is pointing to something
                        } 
                        else 
                        {
                            return 0; // The ast_node_ref is NULL
                        }
                    }
                }
            }
            current = current->next; // Move to the next node
        }

        // If the function reaches this point, it means the identifier wasn't found
        return 0; // Entry not found in the symbol table
    }


    struct ASTNode* GetAssignmentNodeRef(struct SymbolTableNode* head, char itemName[50], char scope[50])
    {
        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                // Inside each node, iterate through the symbol table
                for (int i = 0; i < current->symTabEntryCounter; i++) 
                {
                    if (current->symTableEntryType.globalSymTab[i] != NULL &&
                        strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0) 
                    {
                        return current->symTableEntryType.globalSymTab[i]->assignment_node_ref;
                    }
                    else if (current->symTableEntryType.funcSymTab[i] != NULL &&
                            strcmp(current->symTableEntryType.funcSymTab[i]->itemName, itemName) == 0) 
                    {
                        return current->symTableEntryType.funcSymTab[i]->assignment_node_ref;
                    }
                }
            }
            current = current->next; // Move to the next node
        }

        // If the function reaches this point, it means the identifier wasn't found
        return NULL; // Entry not found in the symbol table
    }


    struct ASTNode* GetASTNodeRef(struct SymbolTableNode* head, char itemName[50], char* scope, bool checkGlobalScope, char indentation[50])
    {
        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                // Inside each node, iterate through the symbol table
                for (int i = 0; i < current->symTabEntryCounter; i++) 
                {
                    if(current->symTableEntryType.globalSymTab[i] != NULL &&
                    strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0)
                    {
                        return current->symTableEntryType.globalSymTab[i]->ast_node_ref;
                    }
                    else if(current->symTableEntryType.funcSymTab[i] != NULL &&
                            strcmp(current->symTableEntryType.funcSymTab[i]->itemName, itemName) == 0)
                    {
                        return current->symTableEntryType.funcSymTab[i]->ast_node_ref;
                    }
                }
            }

            if(checkGlobalScope)
            {
                // Do an aditional check to see whether the identifier is in the global scope
                if (strcmp(current->symTabScope, "Global") == 0) 
                {
                    printf(ANSI_COLOR_GREY "%s--> Searching Global Symbol table for ASTNodeRef of identifier '%s'. \n" ANSI_COLOR_RESET, indentation, itemName);
                    
                    // Inside each node, iterate through the symbol table
                    for (int i = 0; i < current->symTabEntryCounter; i++) 
                    {
                        if(current->symTableEntryType.globalSymTab[i] != NULL &&
                        strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0)
                        {
                            printf(ANSI_COLOR_YELLOW"%s--> Identifier ASTNodeRef found in Global Symbol table '%s'. " ANSI_COLOR_RESET, indentation, itemName);
                            strcpy(scope, "Global");
                            return current->symTableEntryType.globalSymTab[i]->ast_node_ref;
                        }
                    }
                }
            }

            current = current->next; // Move to the next node
        }

        return NULL; // Identifier not found
    }


    int TryIncrementVarNumRef(struct SymbolTableNode* head, char itemName[50], char scope[50])
    {
        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                // Inside each node, iterate through the symbol table
                for (int i = 0; i < current->symTabEntryCounter; i++) 
                {
                    if (current->symTableEntryType.globalSymTab[i] != NULL &&
                        strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0) 
                    {
                        if (current->symTableEntryType.globalSymTab[i]->numRef + 1 >= 2) 
                        {
                            current->symTableEntryType.globalSymTab[i]->is_dead = 0;
                        }
                        current->symTableEntryType.globalSymTab[i]->numRef += 1;
                        return 1;
                    }
                    else if (current->symTableEntryType.funcSymTab[i] != NULL &&
                            strcmp(current->symTableEntryType.funcSymTab[i]->itemName, itemName) == 0) 
                    {
                        if (current->symTableEntryType.funcSymTab[i]->numRef + 1 >= 2) 
                        {
                            current->symTableEntryType.funcSymTab[i]->is_dead = 0;
                        }
                        current->symTableEntryType.funcSymTab[i]->numRef += 1;
                        return 1;
                    }
                }
            }
            current = current->next; // Move to the next node
        }

        // If the function reaches this point, it means the identifier wasn't found
        return 0; // Entry not found in the symbol table
    }


    int TryDecrementVarNumRef(struct SymbolTableNode* head, char itemName[50], char scope[50])
    {
        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                // Inside each node, iterate through the symbol table
                for (int i = 0; i < current->symTabEntryCounter; i++) 
                {
                    if (current->symTableEntryType.globalSymTab[i] != NULL &&
                        strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0) 
                    {
                        if (current->symTableEntryType.globalSymTab[i]->numRef - 1 == 1) 
                        {
                            current->symTableEntryType.globalSymTab[i]->is_dead = 1;
                        }
                        current->symTableEntryType.globalSymTab[i]->numRef -= 1;
                        return 1;
                    }
                    else if (current->symTableEntryType.funcSymTab[i] != NULL &&
                            strcmp(current->symTableEntryType.funcSymTab[i]->itemName, itemName) == 0) 
                    {
                        if (current->symTableEntryType.funcSymTab[i]->numRef - 1 == 1) 
                        {
                            current->symTableEntryType.funcSymTab[i]->is_dead = 1;
                        }
                        current->symTableEntryType.funcSymTab[i]->numRef -= 1;
                        return 1;
                    }
                }
            }
            current = current->next; // Move to the next node
        }

        // If the function reaches this point, it means the identifier wasn't found
        return 0; // Entry not found in the symbol table
    }


    int IsVarDead(struct SymbolTableNode* head, struct ASTNode* ast_node_ref)
    {
        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            // Inside each node, iterate through the symbol table
            for (int i = 0; i < current->symTabEntryCounter; i++) 
            {
                if (current->symTableEntryType.globalSymTab[i] != NULL &&
                    current->symTableEntryType.globalSymTab[i]->ast_node_ref == ast_node_ref) 
                {
                    return current->symTableEntryType.globalSymTab[i]->is_dead;
                }
                else if (current->symTableEntryType.funcSymTab[i] != NULL &&
                        current->symTableEntryType.funcSymTab[i]->ast_node_ref == ast_node_ref) 
                {
                    return current->symTableEntryType.funcSymTab[i]->is_dead;
                }
            }
            current = current->next; // Move to the next node
        }

        // If the function reaches this point, it means the identifier wasn't found
        return 0; // Entry not found in the symbol table
    }


    int SetIsVarDead(struct SymbolTableNode* head, char itemName[50], char scope[50], int value)
    {
    
        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            // Check if the current node has the matching scope
            if (strcmp(current->symTabScope, scope) == 0) 
            {
                // Inside each node, iterate through the symbol table
                for (int i = 0; i < current->symTabEntryCounter; i++) 
                {
                    if (current->tableType == GlobalTable) 
                    {
                        if(strcmp(current->symTableEntryType.globalSymTab[i]->itemName, itemName) == 0)
                        {
                            current->symTableEntryType.globalSymTab[i]->is_dead = value;
                            return 1; // Variable value updated
                        }  
                    }
                    else if (current->tableType == FunctionTable)
                    {
                        if(strcmp(current->symTableEntryType.funcSymTab[i]->itemName, itemName) == 0) 
                        {
                            current->symTableEntryType.funcSymTab[i]->is_dead = value;
                            return 1; // Variable value updated
                        }       
                    }
                }
            }
            current = current->next; // Move to the next node
        }

    
        return 0;
    
    }
    

    void addItemToGlobalSymTab(struct SymbolTableNode* head, char itemName[50], char itemKind[8], char itemType[8], struct ASTNode* ast_node_ref)
    {
        struct SymbolTableNode* globSymTabNode = GetSymbolTableFromLLScope(head, "Global");
        printf(ANSI_COLOR_BRIGHT_YELLOW"\n%s#####- Adding IDENTIFIER '%s' to Global Symbol Table -##### \n" ANSI_COLOR_RESET, "          ", itemName);
        globSymTabNode->symTableEntryType.globalSymTab[globSymTabNode->symTabEntryCounter]->itemID = globSymTabNode->symTabEntryCounter;
        globSymTabNode->symTableEntryType.globalSymTab[globSymTabNode->symTabEntryCounter]->numRef = 0;
        globSymTabNode->symTableEntryType.globalSymTab[globSymTabNode->symTabEntryCounter]->is_dead = 0;
        strcpy(globSymTabNode->symTableEntryType.globalSymTab[globSymTabNode->symTabEntryCounter]->itemName, itemName);
        strcpy(globSymTabNode->symTableEntryType.globalSymTab[globSymTabNode->symTabEntryCounter]->itemKind, itemKind);
        strcpy(globSymTabNode->symTableEntryType.globalSymTab[globSymTabNode->symTabEntryCounter]->itemType, itemType);
        globSymTabNode->symTableEntryType.globalSymTab[globSymTabNode->symTabEntryCounter]->ast_node_ref=ast_node_ref;
        strcpy(globSymTabNode->symTableEntryType.globalSymTab[globSymTabNode->symTabEntryCounter]->itemValue, "");
        globSymTabNode->symTabEntryCounter++;
    }


    void CopyInstructionToSymTab(struct SymbolTableNode* head, struct Instruction instruction, char functionScope[50])
    {
      
        struct SymbolTableNode* funcSymTabNode = GetSymbolTableFromLLScope(head, functionScope);
        if(funcSymTabNode == NULL) { printf(ANSI_COLOR_RED "ERROR: function table is NULL. Head is '%p', scope is '%s' \n" ANSI_COLOR_RESET, head, functionScope); return; }
        struct SymbolTableNode* instructionTable = funcSymTabNode->instructionTable;
        if(funcSymTabNode->instructionTable == NULL) { printf(ANSI_COLOR_RED "ERROR: instructionSymTabNode is NULL \n" ANSI_COLOR_RESET); return; }
        
        
        // Create a new Instruction and copy the values from the original
        struct Instruction* newInstruction = malloc(sizeof(struct Instruction));
        if (newInstruction == NULL) {
            printf(ANSI_COLOR_RED "ERROR: Memory allocation failed for newInstruction\n" ANSI_COLOR_RESET);
            return;
        }

        // Copy all fields from the original Instruction
        memcpy(newInstruction, &instruction, sizeof(struct Instruction));
        // Store the newInstruction pointer in the symbol table
        instructionTable->symTableEntryType.instructionSymTab[instructionTable->symTabEntryCounter] = newInstruction;
        instructionTable->symTabEntryCounter++;
        
    }


    void copyParamInstructionsToStack(struct SymbolTableNode* symbolTableNode) 
    {
        printf(ANSI_COLOR_BRIGHT_YELLOW "\n%s#####- Copy To Stack Details -#####" ANSI_COLOR_RESET, "           " );
        //ReverseFuncInstructionStack();

            
        int counter = 0;
        if (symbolTableNode->tableType != InstructionTable) {
            // Handle incorrect symbol table type
            printf("Error: Symbol table is not of type InstructionTable.\n");
            return;
        }

        for (int i = symbolTableNode->symTabEntryCounter-1; i >= 0; i--) 
        {
            if(symbolTableNode->symTableEntryType.instructionSymTab[i]->instructionType == PARAM_DECLARATION)
            {
                printf(ANSI_COLOR_GREY "\n%s--> Info: Copying instruction '%d' to stack " ANSI_COLOR_RESET, "           ", i);
                PushFuncInstruction(symbolTableNode->symTableEntryType.instructionSymTab[i]);
                counter++;
            }
        }
        printf(ANSI_COLOR_GREY "\n%s--> Info: Copied '%d' instructions to stack \n" ANSI_COLOR_RESET, "           ", counter);
        //ReverseFuncInstructionStack();
    }


    void copyNonParamInstructionsToStack(struct SymbolTableNode* symbolTableNode) 
    {
        printf(ANSI_COLOR_BRIGHT_YELLOW "\n%s#####- Copy To Stack Details -#####" ANSI_COLOR_RESET, "           " );
        //ReverseFuncInstructionStack();

            
        int counter = 0;
        if (symbolTableNode->tableType != InstructionTable) {
            // Handle incorrect symbol table type
            printf("Error: Symbol table is not of type InstructionTable.\n");
            return;
        }

        for (int i = symbolTableNode->symTabEntryCounter-1; i >= 0; i--) 
        {
            if(symbolTableNode->symTableEntryType.instructionSymTab[i]->instructionType != PARAM_DECLARATION)
            {
                printf(ANSI_COLOR_GREY "\n%s--> Info: Copying instruction '%d' to stack " ANSI_COLOR_RESET, "           ", i);
                PushFuncInstruction(symbolTableNode->symTableEntryType.instructionSymTab[i]);
                counter++;
            }
        }
        printf(ANSI_COLOR_GREY "\n%s--> Info: Copied '%d' instructions to stack \n" ANSI_COLOR_RESET, "           ", counter);
        //ReverseFuncInstructionStack();
    }


    void addItemToFuncSymTab(struct SymbolTableNode* head, char functionScope[50], char itemName[50], char itemKind[8], char itemType[8], struct ASTNode* ast_node_ref)
    {
        
        struct SymbolTableNode* funcSymTabNode = GetSymbolTableFromLLScope(head, functionScope);
        printf(ANSI_COLOR_BRIGHT_YELLOW "\n%s #####- Adding IDENTIFIER '%s' to function '%s' symbol table -##### \n" ANSI_COLOR_RESET,"          ", itemName, funcSymTabNode->symTabScope);
        funcSymTabNode->symTableEntryType.funcSymTab[funcSymTabNode->symTabEntryCounter]->itemID = funcSymTabNode->symTabEntryCounter;
        funcSymTabNode->symTableEntryType.funcSymTab[funcSymTabNode->symTabEntryCounter]->numRef = 0;
        funcSymTabNode->symTableEntryType.funcSymTab[funcSymTabNode->symTabEntryCounter]->is_dead = 0;
        strcpy(funcSymTabNode->symTableEntryType.funcSymTab[funcSymTabNode->symTabEntryCounter]->itemName, itemName);
        strcpy(funcSymTabNode->symTableEntryType.funcSymTab[funcSymTabNode->symTabEntryCounter]->itemKind, itemKind);
        strcpy(funcSymTabNode->symTableEntryType.funcSymTab[funcSymTabNode->symTabEntryCounter]->itemType, itemType);
        funcSymTabNode->symTableEntryType.funcSymTab[funcSymTabNode->symTabEntryCounter]->ast_node_ref=ast_node_ref;
        strcpy(funcSymTabNode->symTableEntryType.funcSymTab[funcSymTabNode->symTabEntryCounter]->itemValue, "null");
        funcSymTabNode->symTabEntryCounter++;
    }


    void AddNextLiveIDENTIFIERToDictonary(struct SymbolTableNode* head, char functionScope[50], int* index)
    {
        struct SymbolTableNode* current = GetSymbolTableFromLLScope(head, functionScope);

        int counter = 0;
        /// summary:
        /// So add the IDENTIFIER, value key pair to the dictionary.
        int k = *index;

        for (int i = k; i < current->symTabEntryCounter; i++)
        {
            if (current->tableType == GlobalTable) 
            {
                if(current->symTableEntryType.globalSymTab[i]->is_dead == 0 && strcmp(current->symTableEntryType.globalSymTab[i]->itemKind, "Param") != 0)
                {  
                    *index = i + 1;
               
                    char key[50];
                    int value;

                    strcpy(key, current->symTableEntryType.globalSymTab[i]->itemName);
                    value = atoi(TryGetVarValue(head, current->symTableEntryType.globalSymTab[i]->itemName, functionScope));
                    
                    addToDictionary(&myDict, key, value);
                    printf(ANSI_COLOR_GREY "\n%s--> Info: The IDENTIFIER '%s' is was added to the dictionary as a key with value '%d'" ANSI_COLOR_RESET, indentation2, key, value);
                    counter++;
                }
            }
            else if (current->tableType == FunctionTable)
            {
                if(current->symTableEntryType.funcSymTab[i]->is_dead == 0 && strcmp(current->symTableEntryType.globalSymTab[i]->itemKind, "Param") != 0) 
                {
                    *index = i + 1;

                    char key[50];
                    int value;

                    strcpy(key,  current->symTableEntryType.funcSymTab[i]->itemName);
                    value = atoi(TryGetVarValue(head,  current->symTableEntryType.funcSymTab[i]->itemName, functionScope));


                    addToDictionary(&myDict, key, value);
                    printf(ANSI_COLOR_GREY "\n%s--> Info: The IDENTIFIER '%s' is was added to the dictionary as a key with value '%d'" ANSI_COLOR_RESET, indentation2, key, value);
                    counter++;
                }       
            }
        }

        if(counter == 0) { printf(ANSI_COLOR_BRIGHT_RED "\n%s--> POTENTIAL WARNING: No IDENTIFIER was to add to dictionary" ANSI_COLOR_RESET, indentation2); }
    }

    
    int GetNumberOfLiveIDENTIFIERS(struct SymbolTableNode* head, char functionScope[50], bool includeParams)
    {
        struct SymbolTableNode* current = GetSymbolTableFromLLScope(head, functionScope);
        int counter = 0;

        // Inside each node, iterate through the symbol table
        for (int i = 0; i < current->symTabEntryCounter; i++) 
        {
            if (current->tableType == GlobalTable) 
            {
                if(current->symTableEntryType.globalSymTab[i]->is_dead == 0 && includeParams == true)
                {  
                    counter++;
                }
                else if(current->symTableEntryType.globalSymTab[i]->is_dead == 0 && strcmp(current->symTableEntryType.globalSymTab[i]->itemKind, "Param") != 0)
                {  
                    counter++;
                }
            }
            else if (current->tableType == FunctionTable)
            {
                if(current->symTableEntryType.funcSymTab[i]->is_dead == 0 && includeParams == true)
                {  
                    counter++;
                }
                else if(current->symTableEntryType.funcSymTab[i]->is_dead == 0 && strcmp(current->symTableEntryType.funcSymTab[i]->itemKind, "Param") != 0) 
                {
                    counter++;
                }       
            }
        }
        return counter;
    }


    void PerformLivenessAnalysis(struct SymbolTableNode* head)
    {
        int counter = 0;
        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        printf(ANSI_COLOR_ORANGE "\n #####- Performing Liveness Analysis -##### \n" ANSI_COLOR_RESET);

        while (current != NULL) 
        {
            // Inside each node, iterate through the symbol table
            for (int i = 0; i < current->symTabEntryCounter; i++) 
            {
                switch(current->tableType){
                    case GlobalTable:
                        if(current->symTableEntryType.globalSymTab[i]->numRef == 1 || current->symTableEntryType.globalSymTab[i]->numRef == 0 && current->symTableEntryType.globalSymTab[i]->is_dead == 0 && strcmp(current->symTableEntryType.globalSymTab[i]->itemName, "") != 0)
                        {
                            printf(ANSI_COLOR_ORANGE "\n --> Variable '%s' was marked dead " ANSI_COLOR_RESET, current->symTableEntryType.globalSymTab[i]->itemName);
                            current->symTableEntryType.globalSymTab[i]->is_dead = 1;
                            counter++;
                        }
                        break;
                    case FunctionTable:
                        if(current->symTableEntryType.funcSymTab[i]->numRef == 1 || current->symTableEntryType.funcSymTab[i]->numRef == 0 && current->symTableEntryType.funcSymTab[i]->is_dead == 0 && strcmp(current->symTableEntryType.funcSymTab[i]->itemName, "") != 0)
                        {
                            printf(ANSI_COLOR_ORANGE "\n --> Variable '%s' was marked dead " ANSI_COLOR_RESET, current->symTableEntryType.funcSymTab[i]->itemName);
                            current->symTableEntryType.funcSymTab[i]->is_dead = 1;
                            counter++;
                        }
                        break;
                    default:
                        break;
                }
            }
            current = current->next; // Move to the next node
        }

        // After need to do a second pass to mark any functions as alive if any function variables are alive.

        current = head;

        while (current != NULL) 
        {
            bool alive = false;

            // Inside each node, iterate through the symbol table
            for (int i = 0; i < current->symTabEntryCounter; i++) 
            {
       
                switch(current->tableType){
                    case GlobalTable:
                        /*if(current->symTableEntryType.globalSymTab[i]->is_dead == 0)
                        {
                            alive = true;
                        }*/
                        break;
                    case FunctionTable:
                        if(current->symTableEntryType.funcSymTab[i]->is_dead == 0)
                        {
                            printf(ANSI_COLOR_ORANGE "\n --> IDENTIFIER '%s' at index '%d' in scope '%s' is alive. " ANSI_COLOR_RESET, current->symTableEntryType.funcSymTab[i]->itemName, i, current->symTabScope);
                            alive = true;
                        }
                        break;
                    default:
                        break;
                }
            }

            if(alive == true)
            {  
                // TryIncrementVarNumRef(struct SymbolTableNode* head, char itemName[50], char scope[50])
                int updatedNumRef = TryIncrementVarNumRef(head, current->symTabScope, "Global");
                int updatedIsDead = SetIsVarDead(head, current->symTabScope, "Global", 0);
                if(updatedNumRef == 1 && updatedIsDead == 1)
                {
                    printf(ANSI_COLOR_ORANGE "\n --> Function '%s' was marked alive " ANSI_COLOR_RESET, current->symTabScope);
                }else
                {
                    printf(ANSI_COLOR_BRIGHT_RED "\n Function '%s' failed to be set to alive status " ANSI_COLOR_RESET, current->symTabScope);
                }
            }

            current = current->next; // Move to the next node
        }



        printf(ANSI_COLOR_ORANGE "\n\n #####- Number of IDENTIFIERS marked dead -> '%d' Analysis End. -##### \n" ANSI_COLOR_RESET, counter);
    }


    void getStringBeforeColon(const char* line, char* result) 
    {
        int i = 0;

        while (line[i] != ':' && line[i] != '\0') {
            result[i] = line[i];
            i++;
        }

        result[i] = '\0';
    }


    void removeHashIdentifier(char* inputString) {
        // Find the first non-space character
        char* nonSpaceChar = inputString;
        while (*nonSpaceChar != '\0' && *nonSpaceChar == ' ') {
            nonSpaceChar++;
        }

        // Check if the line starts with "#"
        if (*nonSpaceChar == '#') {
            char* spacePos = strchr(nonSpaceChar, ' ');
            if (spacePos != NULL) {
                // Copy the rest of the string over the '#identifier'
                strcpy(nonSpaceChar, spacePos + 1);
            } else {
                // If no space is found, remove everything after '#'
                *nonSpaceChar = '\0';
            }
        }
    }


    void HandleIRCodeForParameters(struct SymbolTableNode* head)
    {
        int counter = 0;
        char line[100];

        // Iterate through the linked list
        struct SymbolTableNode* current = head;

        printf(ANSI_COLOR_ORANGE "\n #####- Handling IRCode For Parameters -##### \n" ANSI_COLOR_RESET);

        // Open the IR code file for reading
        FILE *fileIR = fopen("OutputFiles/IRCode.ir", "r"); 
        if (fileIR == NULL) {
            printf(ANSI_COLOR_BRIGHT_RED "Error opening 'OutputFiles/IRCode.ir' file for reading.\n" ANSI_COLOR_RESET);
            return;

        }

        printf(ANSI_COLOR_GREY "\n%s--> Info: reading IRCode from file. " ANSI_COLOR_RESET, indentation2);
        
        //readIRCodeFromFile();

        char currentScope[50];
        char functionCall[50];
        bool checkForParam = false;
        bool printedLine = false;
        int numLiveVars = 0;
        bool prevLineFunDef = false;

        // Iterate through the lines of the IR code
        while (fgets(line, 100, fileIR) != NULL) 
        {   
            char* linecopy = strdup(line);

            //printf(ANSI_COLOR_GREY "\n%s--> Info: Current Line '%s'", indentation2, linecopy);

      
            // Find the first non-space character
            char* nonSpaceChar = line;
            while (*nonSpaceChar != '\0' && *nonSpaceChar == ' ') {
                nonSpaceChar++;
            }

            
            // Check if the line starts with "#ParamSet"
            if (strncmp(nonSpaceChar, "Return To Caller", strlen("Return To Caller")) == 0 && prevLineFunDef) 
            {
                printf(ANSI_COLOR_GREY "\n%s--> Info: removing empty function. " ANSI_COLOR_RESET, indentation2);
                printedLine = true;
                prevLineFunDef = false;
            } 
            else if(prevLineFunDef && strncmp(nonSpaceChar, "Return To Caller", strlen("Return To Caller")) != 0) 
            {
                char funcdef[50];
                strcpy(funcdef, currentScope);
                pushIRCodeStack(strncat(funcdef, ":\n", 3));
                prevLineFunDef = false;
            }

        

            // Check if the line starts with a string followed by ":"
            char* colonPos = strchr(nonSpaceChar, ':');
            if (colonPos != NULL && *(colonPos + 1) == '\n') {
                *colonPos = '\0'; // Replace ':' with null terminator
                strcpy(currentScope, nonSpaceChar);  // Update currentScope
                prevLineFunDef = true;
                printedLine = false;
            }


           

            // Check if the line starts with "# "
            if (strncmp(nonSpaceChar, "#", 1) == 0 && checkForParam == true) 
            {
                char* nextString = strtok(nonSpaceChar + 1, " ");
                // Process 'nextString'
                if (nextString != NULL) 
                {
                    printf(ANSI_COLOR_GREY "\n%s--> Info: Recognized Param IDENTIFIER '%s' ", indentation2, nextString);

                    //Check whether the IDENTIFIER is alive in the symbol table
                    int isDead = IsVarDead(head, GetASTNodeRef(head, nextString, functionCall, true, ""));
                    printf(ANSI_COLOR_GREY "\n%s%s--> Info: IDENTIFIER '%s' is dead status is '%d' ", indentation2, indentation2, nextString, isDead);

                    if(isDead == 0)
                    {
                        removeHashIdentifier(linecopy);
                        pushIRCodeStack(linecopy);
                        printedLine = true;
                    }
                       
                }
            }

            // Check if the line starts with "#ParamSet"
            if (strncmp(nonSpaceChar, "#ParamSet", strlen("#ParamSet")) == 0) 
            {
               // Find the next non-space string after "#ParamSet"
                char* nextString = strtok(nonSpaceChar + strlen("#ParamSet"), " ");
                if (nextString != NULL) {
                    // Push 'nextString' onto the temporary stack
                    // tempStackPush(nextString);
                    printf(ANSI_COLOR_GREY "\n%s--> Info: Recognized KeyWord #ParamSet for function '%s' within scope '%s' ", indentation2, nextString, currentScope);
                    

                    int numLiveVars = GetNumberOfLiveIDENTIFIERS(head,  nextString, true);
                    printf(ANSI_COLOR_GREY "\n%s%s--> Info: Number of live IDENTIFIERS in function scioe '%s'  was '%d' ", indentation2, indentation2, nextString, numLiveVars);
                    if(numLiveVars == 0)
                    {
                        printedLine = true;
                        checkForParam = false;
                    }
                    else
                    {
                        checkForParam = true;
                    }
                  
                    strcpy(functionCall, nextString);
                }
            }

  
        

            if(printedLine == false && prevLineFunDef == false)
            {
                pushIRCodeStack(linecopy);
            }

            // Check if the line starts with "#ParamSet"
            if (strncmp(nonSpaceChar, "Jump", strlen("Jump")) == 0) 
            {
                // Formating
                printf("\n");

                char* nextString = strtok(nonSpaceChar + strlen("Jump"), " ");
                if (nextString != NULL) {
                     checkForParam = false;

                    int numLiveVars = GetNumberOfLiveIDENTIFIERS(head,  nextString, true);
                    if(numLiveVars != 0)
                    {
                        pushIRCodeStack(linecopy);
                    }
                }

                printedLine = false;
            }
            
        }



        /*
        
                    struct SymbolTableNode* current = GetSymbolTableFromLLScope(head, nextString);
                    counter = 0;

                    
                    // Inside each node, iterate through the symbol table
                    for (int i = 0; i < current->symTabEntryCounter; i++) 
                    {
                        switch(current->tableType){
                            case GlobalTable:
                                if(current->symTableEntryType.globalSymTab[i]->is_dead == 0 && strcmp(current->symTableEntryType.globalSymTab[i]->itemKind, "Param") == 0)
                                {
                                    // If the param is alive after the liveness analysis, then we need to add the IR code to the function
                                    printf(ANSI_COLOR_ORANGE "\n Error: Somehow a global IDENTIFIER has been marked Param. Call: HandleIRCodeForParameters()  " ANSI_COLOR_RESET, current->symTableEntryType.globalSymTab[i]->itemName);

                                }
                                break;
                            case FunctionTable:
                                if(current->symTableEntryType.funcSymTab[i]->is_dead == 0 && strcmp(current->symTableEntryType.funcSymTab[i]->itemKind, "Param") == 0) 
                                {
                                    printf(ANSI_COLOR_ORANGE "\n%s--> Info: Generating IRCode for param IDENTIFIER '%s' in scope '%s' " ANSI_COLOR_RESET, indentation2, current->symTableEntryType.funcSymTab[i]->itemName, currentScope);


                                    // Get the value of the variable from the 

                                    char paramInstruction[100];
                                    sprintf(paramInstruction, "    Param %s = %d \n", current->symTableEntryType.funcSymTab[i]->itemName, atoi(current->symTableEntryType.funcSymTab[i]->itemValue));
                                    pushIRCodeStack(paramInstruction);
                                    counter++;
                                }
                                break;
                            default:
                                break;
                        }
                    }

                    printf(ANSI_COLOR_GREY "\n%s--> Info: Number of Params IRCode was generated for function '%s' within scope '%s' was '%d' ", indentation2, nextString, currentScope, counter);*/

        printf(ANSI_COLOR_ORANGE "\n\n #####- Done Genertation IRCode For Parameters -##### \n" ANSI_COLOR_RESET);

        fclose(fileIR);

        FILE *file;

        // appead to the current file
        file = fopen("IRCode.ir", "w"); 
        
        if (file == NULL) {
            printf("Error opening IR.ir\n");
            return;
        }

        printIRCodeStackContentsToFile(file);

        fclose(file);
    }


    void RemoveFromSymbolTable(struct SymbolTableNode* head, char scope[50], int index) 
    {
        struct SymbolTableNode* symbolTable = GetSymbolTableFromLLScope(head, scope);

        if (symbolTable == NULL)
        {
            printf(ANSI_COLOR_BRIGHT_RED "\n ERROR: Symbol table with scope '%s' not found in linked list. \n" ANSI_COLOR_RESET, scope);
            return;
        }

        if (index < 0 || index >= 100)
        {
            printf(ANSI_COLOR_BRIGHT_RED "\n ERROR: Invalid index provided. Index must be between 0 and 99. \n" ANSI_COLOR_RESET);
            return;
        }

        if(symbolTable->tableType == GlobalTable)
        {
            if (symbolTable->symTableEntryType.globalSymTab[index] == NULL)
            {
                printf(ANSI_COLOR_BRIGHT_RED "\n ERROR: No entry found at index %d in symbol table. \n" ANSI_COLOR_RESET, index);
                return;
            }

            // Free the memory for the removed entry
            free(symbolTable->symTableEntryType.globalSymTab[index]);

            // Shift elements up to fill the gap
            for (int i = index; i < 99; i++) 
            {
                symbolTable->symTableEntryType.globalSymTab[i] = symbolTable->symTableEntryType.globalSymTab[i + 1];
            }

            // Set the last entry to NULL
            symbolTable->symTableEntryType.globalSymTab[99] = NULL;

            symbolTable->symTabEntryCounter--;
            printf(ANSI_COLOR_BRIGHT_GREEN "\n Entry at index %d in symbol table with scope '%s' removed successfully. \n" ANSI_COLOR_RESET, index, scope);
        }
        else if(symbolTable->tableType == FunctionTable)
        {
            if (symbolTable->symTableEntryType.funcSymTab[index] == NULL)
            {
                printf(ANSI_COLOR_BRIGHT_RED "\n ERROR: No entry found at index %d in symbol table. \n" ANSI_COLOR_RESET, index);
                return;
            }

            // Free the memory for the removed entry
            free(symbolTable->symTableEntryType.funcSymTab[index]);

            // Shift elements up to fill the gap
            for (int i = index; i < 99; i++) 
            {
                symbolTable->symTableEntryType.funcSymTab[i] = symbolTable->symTableEntryType.funcSymTab[i + 1];
            }

            // Set the last entry to NULL
            symbolTable->symTableEntryType.funcSymTab[99] = NULL;
            
            symbolTable->symTabEntryCounter--;
            printf(ANSI_COLOR_BRIGHT_GREEN "\n Entry at index %d in symbol table with scope '%s' removed successfully. \n" ANSI_COLOR_RESET, index, scope);

        }
        else if(symbolTable->tableType == InstructionTable)
        {
            printf(ANSI_COLOR_BRIGHT_RED "\n ERROR: Symbol table type InstructionTable not currently handled. \n" ANSI_COLOR_RESET);
        }
        else
        {
            printf(ANSI_COLOR_BRIGHT_RED "\n ERROR: Symbol table type not recognised. \n" ANSI_COLOR_RESET);
            return;
        }


    }


    void CleanUpAst(struct SymbolTableNode* head, struct ASTNode* root)
    {
        printf(ANSI_COLOR_ORANGE "\n #####- Performing Clean Up of AST -##### \n" ANSI_COLOR_RESET);

        // Delete any functions that have a null return.
        // Get Next function definition.
        //struct ASTNode* GetASTNodeRef(struct SymbolTableNode* head, char itemName[50], char* scope, bool checkGlobalScope, char indentation[50])

        // loop through the global symbol table
        struct SymbolTableNode* globalSymTabNode = GetSymbolTableFromLLScope(head, "Global");

        for (int i = 0; i < globalSymTabNode->symTabEntryCounter; i++) 
        {
            // Check if the current node has the matching type
            if (strcmp(globalSymTabNode->symTableEntryType.globalSymTab[i]->itemKind, "Func") == 0) 
            {
             
                /// summary:
                /// Get the return node for the function, as previously used
                /// 
                /// Need to get reference to the function return node. 
                /// The path to the return node is as follows:
                /// 1.) Get the ASTNode ref from the global symbol table for the function
                /// 2.) The ref->right points to the Block
                /// 3.) The Block->right points to the Return node
                ///
                ///  struct ASTNode* functionNode = GetASTNodeRef(head, funcInstruction->itemName, globScope, false, indentation);
                ///  if(functionNode == NULL) { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: functionNode is NULL \n" ANSI_COLOR_RESET); return false; }
                ///  struct ASTNode* returnNode = functionNode->right->right;
                ///  if(returnNode == NULL) { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: returnNode is NULL \n" ANSI_COLOR_RESET); return false; }
                ///  copy the value of the return node to the assignment node
                ///  The return node itself just holds a reference to the NonAssignExpr
                ///
                ///  This is an example of how it has been previously handled:
                ///  char str[50];	
                ///  strcpy(str, varDeclInstruction->child_ast_node_ref->value);
                ///  varDeclInstruction->assignmentNodeRef->right = varDeclInstruction->child_ast_node_ref;
                ///  char str[50];	
                ///  strcpy(str, returnNode->right->value);
                ///  printf(ANSI_COLOR_GREY "\n%s --> Info: The value stored in the returnNode is '%s'." ANSI_COLOR_RESET, indentation, str);
                ///
                ///  strcpy(funcInstruction->ast_node_ref->value, str);
                ///   
                ///  printf(ANSI_COLOR_GREY "\n%s --> Info: The value stored in the ASTNode constant node created while parsing is now '%s'." ANSI_COLOR_RESET, indentation, funcInstruction->ast_node_ref->value);   

                // Get the ASTNodeRef for the function
                struct ASTNode* functionRootNode = GetASTNodeRef(head, globalSymTabNode->symTableEntryType.globalSymTab[i]->itemName, "Global", false, "          ");
                if(functionRootNode == NULL) { printf(ANSI_COLOR_RED "ERROR: functionRootNode is NULL \n" ANSI_COLOR_RESET); return; }
                struct ASTNode* returnNode = functionRootNode->right->right;
                if(returnNode == NULL) { printf(ANSI_COLOR_BRIGHT_RED "\nERROR: returnNode is NULL \n" ANSI_COLOR_RESET); return; }
                
                // Check if the return node value is null
                if(strcmp(returnNode->right->value, "null") == 0 || strcmp(returnNode->right->value, "Null") == 0)
                {
                    // Delete the function
                    printf(ANSI_COLOR_ORANGE "\n --> Function '%s' has a null return. Deleting function. \n" ANSI_COLOR_RESET, globalSymTabNode->symTableEntryType.globalSymTab[i]->itemName);
                    freeAST(functionRootNode);

                    RemoveFromSymbolTable(head, "Global", i);

                    // Run an aditional pass to remove any empty nodes that were missed. 
                    if(root == NULL){ printf(ANSI_COLOR_RED "ERROR: root is NULL \n" ANSI_COLOR_RESET); return; }
                    DeleteEmptyNodesOfType(root, root, NODE_FUNC_DECL, false);
                }
            }
        }

        printf(ANSI_COLOR_ORANGE "\n #####- Done With Clean Up of AST -##### \n" ANSI_COLOR_RESET);
    }



    
#endif
