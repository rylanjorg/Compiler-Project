#ifndef WHILE_LOOP_SEMANTIC_CHECKS_H
#define WHILE_LOOP_SEMANTIC_CHECKS_H

    #include "../AST/AST.h" 
    #include "../Helpers/HashSet.h"
    #include "../SymbolTableManagement/SymbolTableManager.h"

    int MAX_WHILE_BLOCK_SIZE = 20;

    // Function to analyze conditional expression dependencies
    void analyzeConditionalExpression(struct ASTNode* conditionNode, struct HashSet* dependencies) 
    {
        if (conditionNode != NULL) {
            // Assume 'value' contains the content of the node
            if (conditionNode->node_type == NODE_VARIABLE) {
                // If the node is an identifier, add it to the set of dependencies
                addToHashSet(dependencies, conditionNode->value);
            }

            // Recursively analyze left and right subexpressions
            analyzeConditionalExpression(conditionNode->left, dependencies);
            analyzeConditionalExpression(conditionNode->right, dependencies);
        }
    }

    void AnalyzeWhileLoopSemantics(struct ASTNode* whileNode, struct ASTNode* conditionNode, struct ASTNode* bodyNode)
    {
        /*
            ○ Is the limit of the while loop a constant?
            ○ Is there a non-determinant conditional within the while statement?
            ○ Is there a non-determinant conditional within the while statement that impacts the dependent variable of the while loop?
            ○ Does the while loop have a large body? [define some max unroll size]
            Does the while loop have loop-caried dependencies?
        */


        // Simplify while loop to base structure

        // 1. Analyze dependencies
        struct HashSet* dependencies = createHashSet();
        analyzeConditionalExpression(conditionNode, dependencies);

    }

#endif