
#ifndef CONSTANT_OPTIMIZATIONS_H
#define CONSTANT_OPTIMIZATIONS_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "../Helpers/ConsoleFormating.h"

    char indentationCO[50] = "           ";

    // Define the possible operators for binary operations
    typedef enum 
    {
        INT,
        FLOAT,
        CHAR,
        NULL_TYPE,
        ARRAY_TEMP_ELEMENT,
    } DataType;


    DataType CastStringToDataType(char* type)
    {
        printf(ANSI_COLOR_GREY "\n%s--> Info: Trying to cast string '%s' to Data Type " ANSI_COLOR_RESET, indentationCO, type);
        if(strcmp(type, "int") == 0)
        {
            printf(ANSI_COLOR_GREY "\n%s--> Info: Data Type INT recognized " ANSI_COLOR_RESET, indentationCO);
            return INT;
        }
        else if(strcmp(type, "float") == 0)
        {
            printf(ANSI_COLOR_GREY "\n%s--> Info: Data Type FLOAT recognized " ANSI_COLOR_RESET, indentationCO);
            return FLOAT;
        }
        else if(strcmp(type, "char") == 0)
        {
            printf(ANSI_COLOR_GREY "\n%s--> Info: Data Type CHAR recognized " ANSI_COLOR_RESET, indentationCO);
            return CHAR;
        }
        else
        {
            printf(ANSI_COLOR_BRIGHT_RED "\nError: Cannot cast string to data type.\n" ANSI_COLOR_RESET);
            return NULL_TYPE;
        }
    }

    const char* CastDataTypeToString(DataType type) {
    switch (type) {
        case INT:
            return "int";
        case FLOAT:
            return "float";
        case CHAR:
            return "char";
        default:
            return "unknown";
    }
}




    // Define a structure for storing the value and type relationship for an identifier
    struct PropogationIdentifier
    {
        char value[50];
        DataType type;
    };



    // Function to initialize PropogationIdentifier
    struct PropogationIdentifier* initPropogationIdentifier(char value[50], DataType type)
    {
        struct PropogationIdentifier* propogationIdentifier = (struct PropogationIdentifier*)malloc(sizeof(struct PropogationIdentifier));

        strcpy(propogationIdentifier->value, value);
        propogationIdentifier->type = type;
        return propogationIdentifier;
    }




    // Fold Int values
    char* FoldInt(struct PropogationIdentifier* left, struct PropogationIdentifier* right, char op[8])
    {
        int leftValue = atoi(left->value);
        int rightValue = atoi(right->value);
        int result;

        if(strcmp(op, "+") == 0)
        {
            result = leftValue + rightValue;
        }
        else if(strcmp(op, "-") == 0)
        {
            result = leftValue - rightValue;
        }
        else if(strcmp(op, "*") == 0)
        {
            result = leftValue * rightValue;
        }
        else if(strcmp(op, "/") == 0)
        {
            result = leftValue / rightValue;
        }

        
        char* resultString = (char*)malloc(sizeof(char) * 50);
        sprintf(resultString, "%d", result);
        printf(ANSI_COLOR_GREY "\n%s--> Info: Int Folded '%s %s %s = %s' " ANSI_COLOR_RESET, indentationCO, left->value, op, right->value, resultString);
        return resultString;
    }

    // Fold Float values
    char* FoldFloat(struct PropogationIdentifier* left, struct PropogationIdentifier* right, char op[8])
    {
        float leftValue = atof(left->value);
        float rightValue = atof(right->value);
        float result;

        if(strcmp(op, "+") == 0)
        {
            result = leftValue + rightValue;
        }
        else if(strcmp(op, "-") == 0)
        {
            result = leftValue - rightValue;
        }
        else if(strcmp(op, "*") == 0)
        {
            result = leftValue * rightValue;
        }
        else if(strcmp(op, "/") == 0)
        {
            result = leftValue / rightValue;
        }

        char* resultString = (char*)malloc(sizeof(char) * 50);
        sprintf(resultString, "%f", result);
        printf(ANSI_COLOR_GREY "\n%s--> Info: Float Folded '%s %s %s = %s' " ANSI_COLOR_RESET, indentationCO, left->value, op, right->value, resultString);
        return resultString;
    }


    // Fold Char values only support + opperation where it appends two chars together
    char* FoldChar(struct PropogationIdentifier* left, struct PropogationIdentifier* right, char op[8])
    {
        char* resultString = (char*)malloc(sizeof(char) * 50);
        if(strcmp(op, "+") == 0)
        {
            sprintf(resultString, "%s%s", left->value, right->value);
            printf(ANSI_COLOR_GREY "\n%s--> Info: Char Folded '%s %s %s = %s' " ANSI_COLOR_RESET, indentationCO, left->value, op, right->value, resultString);
            return resultString;
        }
        else
        {
            printf("Char type does not support this opperation.\n");
            exit(1);
        }
    }



    void TypeCast(struct PropogationIdentifier* from, struct PropogationIdentifier* to)
    {
        
        switch (from->type)
        {
            case INT:
                if(to->type == FLOAT)
                {
                    int fromValue = atoi(from->value);
                    float result = (float)fromValue;
                    sprintf(from->value, "%f", result);
                    from->type = FLOAT;
                    printf(ANSI_COLOR_GREY "\n%s--> Info: Type Cast from Int '%d' to Float '%s' " ANSI_COLOR_RESET, indentationCO, fromValue, from->value);
                }
                else if(to->type == CHAR)
                {
                    int fromValue = atoi(from->value);
                    char result = (char)fromValue;
                    sprintf(from->value, "%c", result);
                    from->type = CHAR;
                    printf(ANSI_COLOR_GREY "\n%s--> Info: Type Cast from Int '%d' to Char '%s' " ANSI_COLOR_RESET, indentationCO, fromValue, from->value);
                }
                break;
            case FLOAT:
                if(to->type == INT)
                {
                    float fromValue = atof(from->value);
                    int result = (int)fromValue;
                    sprintf(from->value, "%d", result);
                    from->type = INT;
                    printf(ANSI_COLOR_GREY "\n%s--> Info: Type Cast from Float '%f' to Int '%s' " ANSI_COLOR_RESET, indentationCO, fromValue, from->value);
                }
                else if(to->type == CHAR)
                {
                    float fromValue = atof(from->value);
                    char result = (char)fromValue;
                    sprintf(from->value, "%c", result);
                    from->type = CHAR;
                    printf(ANSI_COLOR_GREY "\n%s--> Info: Type Cast from Float '%f' to Char '%s' " ANSI_COLOR_RESET, indentationCO, fromValue, from->value);
                }
                break;
            case CHAR:
                if(to->type == INT)
                {
                    char fromValue = from->value[0];
                    int result = (int)fromValue;
                    sprintf(from->value, "%d", result);
                    from->type = INT;
                    printf(ANSI_COLOR_GREY "\n%s--> Info: Type Cast from Char '%s' to Int '%s' " ANSI_COLOR_RESET, indentationCO, fromValue, from->value);
                }
                else if(to->type == FLOAT)
                {
                    char fromValue = from->value[0];
                    float result = (float)fromValue;
                    sprintf(from->value, "%f", result);
                    from->type = FLOAT;
                    printf(ANSI_COLOR_GREY "\n%s--> Info: Type Cast from Char '%s'  to Float '%s' " ANSI_COLOR_RESET, indentationCO, fromValue, from->value);
                }
                break;
            default:
                printf(ANSI_COLOR_BRIGHT_RED "\n%s--> Info: Type Cast unrecognized from type" ANSI_COLOR_RESET, indentationCO);
                break;
        }
    }


            
    char* PerformConstantFolding(struct PropogationIdentifier* left, struct PropogationIdentifier* right, char op[8])
    {
        char* resultString;

        if(left == NULL || right == NULL)
        {
            printf("Error: Cannot perform constant propogation on NULL values.\n");
            exit(1);
        } 

        if(left->type != right->type)
        {
            // Type cast with the right type to the left type
            TypeCast(right, left);
        }

        // Check if the types are the same
        if(left->type == right->type)
        {
            switch (left->type)
            {
                case INT:
                    resultString = FoldInt(left, right, op);
                    break;
                case FLOAT:
                    resultString = FoldFloat(left, right, op);
                    break;
                case CHAR:
                    resultString = FoldChar(left, right, op);
                    break;
                default:
                    break;
            }
        }

        return resultString;
    }






            
    /*
    char fc_str[50];	
    int folded_constant;

    if(strcmp(instruction->itemKind, "+") == 0)
    {
        folded_constant = atoi(instruction->itemName) + atoi(instruction->child_ast_node_ref->value);
    }
    else if(strcmp(instruction->itemKind, "-") == 0)
    {
        folded_constant = atoi(instruction->itemName) - atoi(instruction->child_ast_node_ref->value);
    }
    else if(strcmp(instruction->itemKind, "*") == 0)
    {
        folded_constant = atoi(instruction->itemName) * atoi(instruction->child_ast_node_ref->value);
    }
    else if(strcmp(instruction->itemKind, "/") == 0)
    {
        folded_constant = atoi(instruction->itemName) / atoi(instruction->child_ast_node_ref->value);
    }


    sprintf(fc_str, "%d", folded_constant);

    printf(ANSI_COLOR_BRIGHT_MAGENTA "\n%s--> Resolve: Constant folded: '%d' %s '%d' = '%d' \n" ANSI_COLOR_RESET,  "           ", atoi(instruction->itemName), instruction->itemKind, atoi(instruction->child_ast_node_ref->value), folded_constant);
    */

#endif