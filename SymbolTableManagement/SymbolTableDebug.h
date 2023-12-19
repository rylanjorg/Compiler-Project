  

#ifndef SYMBOL_TABLE_DEBUG_H
#define SYMBOL_TABLE_DEBUG_H

    #include "SymbolTableManager.h"

    void PrintSymbolTableNodeToFile(FILE *file, struct SymbolTableNode* node) 
    {
        if (node == NULL) {
            fprintf(file, "Symbol table node is NULL.\n");
            return;
        }

        fprintf(file, "Symbol Table Node:\n");
        fprintf(file, "Scope: %s\n", node->symTabScope);
        fprintf(file, "Index: %d\n", node->symTabIndex);
        fprintf(file, "Entry Counter: %d\n", node->symTabEntryCounter);
        fprintf(file, "Initialized: %s\n", node->isInitialized ? "true" : "false");

        switch (node->tableType) {
            case GlobalTable:
                fprintf(file, "Table Type: GlobalTable\n");
                // Print specific information for GlobalTable
                break;

            case FunctionTable:
                fprintf(file, "Table Type: FunctionTable\n");
                // Print specific information for FunctionTable
                break;

            case InstructionTable:
                fprintf(file, "Table Type: InstructionTable\n");
                // Print specific information for InstructionTable
                break;

            case ConditionalBlockTable:
                fprintf(file, "Table Type: ConditionalBlockTable\n");
                // Print specific information for ConditionalBlockTable
                break;

            default:
                fprintf(file, "Unknown Table Type\n");
                break;
        }

        // Print additional information as needed

        fprintf(file, "Num Conditional Blocks: %d\n", node->numConditionalBlocks);

        // Recursively print information for linked nodes
        if (node->next != NULL) {
            fprintf(file, "\nNext Node:\n");
            PrintSymbolTableNodeToFile(file, node->next);
        }
    }

    void PrintSymbolTablesToFile() {
        FILE *fileST;

        // "w" means open for writing (creates if not exist)
        fileST = fopen("Debug/SymbolTableLL.txt", "w");

        if (fileST == NULL) {
            printf("Error opening SymbolTableLL.txt file: %s\n", strerror(errno));
            return;
        }

        fprintf(fileST, "\n%s\n", "--- Symbol Table Start ---");

        // Iterate through the linked list and print each node to file
        struct SymbolTableNode* current = head;

        while (current != NULL) 
        {
            PrintSymbolTableNodeToFile(fileST, current);
            fprintf(fileST, "\n");  // Add a newline after each table
            current = current->next; // Move to the next node
        }

        fprintf(fileST, "\n%s\n", "--- Symbol Table End ---");

        fclose(fileST);
    }

#endif