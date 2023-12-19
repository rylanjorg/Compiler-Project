#ifndef IR_CODE_HELPERS_H
#define IR_CODE_HELPERS_H

    #include "../AST/AST.h" 

    char* SearchLeftSubTree(struct ASTNode* node)
    {
        // Base case: if the current node is NULL, return NULL
        if (node == NULL)
        {
            return NULL;
        }

        // Recursive case: search the left subtree
        char* result = SearchLeftSubTree(node->left);

        // If a non-null value is found in the left subtree, return it
        if (result != NULL)
        {
            return result;
        }

        // If the current node has no left children and has a non-null value, return it
        if (node->left == NULL && node->value != NULL)
        {
            return node->value;
        }

        // If no non-null value is found, return NULL
        return NULL;
    }

#endif