#ifndef MIPS_CODE_H
#define MIPS_CODE_H

// -------------------------------- START STACK --------------------------------
#include <string.h>
#define MAX_STACK_SIZE 100

// Define the first stack
char mipsStack[MAX_STACK_SIZE][100];  
int topMIPS = -1;


// Push an element onto the second stack
void pushMIPS(char* element) {
    if (topMIPS < MAX_STACK_SIZE - 1) {
        topMIPS++;
        strcpy( mipsStack[topMIPS], element);
    } else {
        printf("Second stack overflow\n");
    }
}

// Pop an element from the second stack
void popMIPS() {
    if (topMIPS >= 0) {
        topMIPS--;
    } else {
        printf("Second stack underflow\n");
    }
}

// Get the top element from the second stack
char* getMIPS() {
    if (topMIPS >= 0) {
        return mipsStack[topMIPS];
    } else {
        return NULL;
    }
}

// Clear the second stack
void clearMIPS() {
    topMIPS = -1;
}

// -------------------------------- END STACK  --------------------------------

void printMIPSStackContentsToFile(FILE *file) 
{
    char concatenatedStringMIPS[1000];  // Assuming the result string can hold up to 1000 characters
    strcpy(concatenatedStringMIPS, "");

    for (int i = 0; i <= topMIPS; i++) 
    {  // Loop in reverse order
        // Check if the line ends with a colon
        if (strrchr(mipsStack[i], ':') == NULL) {
            strcat(concatenatedStringMIPS, "\t");
        }
        strcat(concatenatedStringMIPS, mipsStack[i]);
    }
    fprintf(file, ".text\n.globl main\n"); // define .text and .globl main
    fprintf(file, "%s", concatenatedStringMIPS);
    fprintf(file, "\tli $v0, 10\t# Exit program\n\tsyscall\n"); // syscall exit
    clearMIPS();
}

#endif