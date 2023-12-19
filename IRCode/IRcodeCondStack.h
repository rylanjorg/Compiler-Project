#ifndef IR_CODE_COND_STACK_H
#define IR_CODE_COND_STACK_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include "../Helpers/ConsoleFormating.h"
    #include "IRcodeStack.h"

    // Define a stack data structure
    #define MAX_STACK_SIZE 100
    #define MAX_IR_CODE_LINE_LENGTH 100
    char IRCodeStack_Conditionals[MAX_STACK_SIZE][MAX_IR_CODE_LINE_LENGTH];  // Assuming each element in the stack can hold 100 characters
    int topIRCodeStack_Conditionals = -1;

    // Push an element onto the stack
    void pushIRCodeStack_Conditionals(char* element) {
        if (topIRCodeStack_Conditionals < MAX_STACK_SIZE - 1) {
            topIRCodeStack_Conditionals++;
            strcpy(IRCodeStack_Conditionals[topIRCodeStack_Conditionals], element);
        } else {
            printf("Stack overflow\n");
        }
    }

    // Pop an element from the stack
    void popIRCodeStack_Conditionals() {
        if (topIRCodeStack_Conditionals >= 0) {
            topIRCodeStack_Conditionals--;
        } else {
            printf("Stack underflow\n");
        }
    }

    // Get the top element from the stack
    char* getTopIRCodeStack_Conditionals() {
        if (topIRCodeStack_Conditionals >= 0) {
            return IRCodeStack_Conditionals[topIRCodeStack_Conditionals];
        } else {
            return NULL;
        }
    }

    // Clear the stack
    void clearIRCodeStack_Conditionals() {
        topIRCodeStack_Conditionals = -1;
    }

    void printIRCodeStackContents_Conditionals() 
    {
        char concatenatedString[1000];  // Assuming the result string can hold up to 1000 characters
        strcpy(concatenatedString, "");

        for (int i = 0; i <= topIRCodeStack_Conditionals; i++) {  // Loop in reverse order
            strcat(concatenatedString, IRCodeStack_Conditionals[i]);
            strcat(concatenatedString, " ");
        }

        printf(ANSI_COLOR_BRIGHT_CYAN "\n %s \n" ANSI_COLOR_RESET, concatenatedString);
        clearIRCodeStack_Conditionals();
    }

    void printIRCodeStackContentsToFile_Conditionals(FILE *file) 
    {
        char concatenatedString[1000];  // Assuming the result string can hold up to 1000 characters
        strcpy(concatenatedString, "");

        for (int i = 0; i <= topIRCodeStack_Conditionals; i++) {  // Loop in reverse order
            fprintf(file, IRCodeStack_Conditionals[i]);
        }

        clearIRCodeStack_Conditionals();
    }

    void printIRCodeStackContentsToMainStack() 
    {
        char concatenatedString[1000];  // Assuming the result string can hold up to 1000 characters
        strcpy(concatenatedString, "");

        for (int i = 0; i <= topIRCodeStack_Conditionals; i++) {  // Loop in reverse order
            pushIRCodeStack(IRCodeStack_Conditionals[i]);
        }

        clearIRCodeStack_Conditionals();
    }

    // Function to read IR code from a file and push it onto the stack
    void readIRCodeFromFile_Conditionals() 
    {
        FILE *file;
        char line[100];

        file = fopen("OutputFiles/IRCode.ir", "r");
        if (file == NULL) {
            printf("Error opening file.\n");
            return;
        }

        while (fgets(line, sizeof(line), file)) {
            pushIRCodeStack_Conditionals(line);
        }

        fclose(file);
    }

#endif