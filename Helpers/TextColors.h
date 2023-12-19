#include <stdio.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
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

void prepend_to_production_rule(char* productionRule, const char* prefix) 
{
    char temp[100];
    strcpy(temp, prefix);
    strcat(temp, productionRule);
    strcpy(productionRule, temp);
}

