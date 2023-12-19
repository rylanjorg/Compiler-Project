#ifndef IR_CODE_STACK_H
#define IR_CODE_STACK_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include "../Helpers/ConsoleFormating.h"

    // Define a stack data structure
    #define MAX_STACK_SIZE 100
    #define MAX_IR_CODE_LINE_LENGTH 100
    char IRCodeStack[MAX_STACK_SIZE][MAX_IR_CODE_LINE_LENGTH];  // Assuming each element in the stack can hold 100 characters
    int topIRCodeStack = -1;

    // Push an element onto the stack
    void pushIRCodeStack(char* element) {
        if (topIRCodeStack < MAX_STACK_SIZE - 1) {
            topIRCodeStack++;
            strcpy(IRCodeStack[topIRCodeStack], element);
        } else {
            printf("Stack overflow\n");
        }
    }

    // Pop an element from the stack
    void popIRCodeStack() {
        if (topIRCodeStack >= 0) {
            topIRCodeStack--;
        } else {
            printf("Stack underflow\n");
        }
    }

    // Get the top element from the stack
    char* getTopIRCodeStack() {
        if (topIRCodeStack >= 0) {
            return IRCodeStack[topIRCodeStack];
        } else {
            return NULL;
        }
    }

    // Clear the stack
    void clearIRCodeStack() {
        topIRCodeStack = -1;
    }

    void printIRCodeStackContents() 
    {
        char concatenatedString[1000];  // Assuming the result string can hold up to 1000 characters
        strcpy(concatenatedString, "");

        for (int i = 0; i <= topIRCodeStack; i++) {  // Loop in reverse order
            strcat(concatenatedString, IRCodeStack[i]);
            strcat(concatenatedString, " ");
        }

        printf(ANSI_COLOR_BRIGHT_CYAN "\n %s \n" ANSI_COLOR_RESET, concatenatedString);
        clearIRCodeStack();
    }

    void printIRCodeStackContentsToFile(FILE *file) 
    {
        char concatenatedString[1000];  // Assuming the result string can hold up to 1000 characters
        strcpy(concatenatedString, "");

        for (int i = 0; i <= topIRCodeStack; i++) {  // Loop in reverse order
            fprintf(file, IRCodeStack[i]);
        }

        clearIRCodeStack();
    }

    // Function to read IR code from a file and push it onto the stack
    void readIRCodeFromFile() 
    {
        FILE *file;
        char line[100];

        file = fopen("OutputFiles/IRCode.ir", "r");
        if (file == NULL) {
            printf("Error opening file.\n");
            return;
        }

        while (fgets(line, sizeof(line), file)) {
            pushIRCodeStack(line);
        }

        fclose(file);
    }

#endif