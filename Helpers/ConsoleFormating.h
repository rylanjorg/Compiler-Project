#ifndef CONSOLE_FORMAT_H
#define CONSOLE_FORMAT_H

#include <stdio.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_GREY    "\x1b[90m"
#define ANSI_COLOR_ORANGE  "\x1b[38;5;208m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define ANSI_COLOR_BRIGHT_BLACK   "\x1b[30;1m"
#define ANSI_COLOR_BRIGHT_RED     "\x1b[31;1m"
#define ANSI_COLOR_BRIGHT_GREEN   "\x1b[32;1m"
#define ANSI_COLOR_BRIGHT_YELLOW  "\x1b[33;1m"
#define ANSI_COLOR_BRIGHT_BLUE    "\x1b[34;1m"
#define ANSI_COLOR_BRIGHT_MAGENTA "\x1b[35;1m"
#define ANSI_COLOR_BRIGHT_CYAN    "\x1b[36;1m"
#define ANSI_COLOR_BRIGHT_WHITE   "\x1b[37;1m"


#define ANSI_COLOR_BRIGHT_BLACK_BG   "\x1b[40;1m"
#define ANSI_COLOR_BRIGHT_RED_BG     "\x1b[41;1m"
#define ANSI_COLOR_BRIGHT_GREEN_BG   "\x1b[42;1m"
#define ANSI_COLOR_BRIGHT_YELLOW_BG  "\x1b[43;1m"
#define ANSI_COLOR_BRIGHT_BLUE_BG    "\x1b[44;1m"
#define ANSI_COLOR_BRIGHT_MAGENTA_BG "\x1b[45;1m"
#define ANSI_COLOR_BRIGHT_CYAN_BG    "\x1b[46;1m"
#define ANSI_COLOR_BRIGHT_WHITE_BG   "\x1b[47;1m"

#define ANSI_COLOR_GREEN_DARK   "\x1B[92m"


void prepend_to_production_rule(char* productionRule, const char* prefix) 
{
    char temp[100];
    strcpy(temp, prefix);
    strcat(temp, productionRule);
    strcpy(productionRule, temp);
}

// Define a stack data structure
#define MAX_STACK_SIZE 100
char outputStack[MAX_STACK_SIZE][100];  // Assuming each element in the stack can hold 100 characters
int top = -1;

// Push an element onto the stack
void push(char* element) {
    if (top < MAX_STACK_SIZE - 1) {
        top++;
        strcpy(outputStack[top], element);
    } else {
        printf("Stack overflow\n");
    }
}

// Pop an element from the stack
void pop() {
    if (top >= 0) {
        top--;
    } else {
        printf("Stack underflow\n");
    }
}

// Get the top element from the stack
char* getTop() {
    if (top >= 0) {
        return outputStack[top];
    } else {
        return NULL;
    }
}

// Clear the stack
void clearStack() {
    top = -1;
}

void printStackContents() 
{
    char concatenatedString[1000];  // Assuming the result string can hold up to 1000 characters
    strcpy(concatenatedString, "");

    for (int i = 0; i <= top; i++) {  // Loop in reverse order
        strcat(concatenatedString, outputStack[i]);
        strcat(concatenatedString, " ");
    }

    printf(ANSI_COLOR_BRIGHT_CYAN "\n RECOGNIZED RULE: %s \n" ANSI_COLOR_RESET, concatenatedString);
    clearStack();
}

void printStackContentsToFile(FILE *file) 
{
    char concatenatedString[1000];  // Assuming the result string can hold up to 1000 characters
    strcpy(concatenatedString, "");

    for (int i = 0; i <= top; i++) {  // Loop in reverse order
        strcat(concatenatedString, outputStack[i]);
        strcat(concatenatedString, " ");
    }

    fprintf(file, "%s", concatenatedString);
    clearStack();
}



#endif

