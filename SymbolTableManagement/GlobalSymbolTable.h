#ifndef GLOBAL_SYMBOL_TABLE_H
#define GLOBAL_SYMBOL_TABLE_H

    #include "SymbolTableManager.h"
    #include "../AST/AST.h"

    char globScope[50];
    struct SymbolTableNode* head = NULL;

#endif



/*
void freeSymbolTableVariables() {
    for (int i = 0; i < symTabIndex; i++) {
        // Free any resources associated with each variable AST node 
        if (symTabItems[i].ast_node_ref != NULL) {
            printf("Freed variable: ");
			printf(symTabItems[i].itemName);
			printf("\n");
			free(symTabItems[i].ast_node_ref);
            symTabItems[i].ast_node_ref = NULL;
        }

    }
}
*/


