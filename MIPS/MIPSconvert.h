#ifndef MIPS_CONVERT_H
#define MIPS_CONVERT_H

void ConvertIRCodeToMIPS()
    {
        FILE *fileMIPS;
        FILE *fileDATA;

        // append to the current file
        fileMIPS = fopen("OutputFiles/MipsScript.s", "w"); 
        fileDATA = fopen("OutputFiles/MipsScriptData.s", "w");
        
        if (fileMIPS == NULL) {
            printf("Error opening MipsScript.s\n");
        }
        if (fileDATA == NULL) {
            printf("Error opening MipsScriptData.s\n");
        }

        printf(ANSI_COLOR_ORANGE "\n #####- Converting IRCode.ir to MIPS -##### \n" ANSI_COLOR_RESET);

        // Open the IR code file for reading
        FILE *fileIR = fopen("OutputFiles/IRCode.ir", "r"); 
        if (fileIR == NULL) {
            printf(ANSI_COLOR_BRIGHT_RED "Error opening IRCode.ir file for reading.\n" ANSI_COLOR_RESET);
        }

        char currentScope[50];
        char functionCall[50];
        bool checkForParam = false;

        char line[256]; // You can adjust the buffer size as needed
        char print_value[256];
        char var_name[256];
        char var_value[256];
        char var_value2[256];

        fprintf(fileDATA, ".data\n");
        fprintf(fileMIPS, ".text\n");
        fprintf(fileMIPS, ".globl main\n\n");


        while (fgets(line, sizeof(line), fileIR) != NULL) {
            // Remove leading whitespace or '\t' characters
            while (*line == ' ' || *line == '\t') {
                memmove(line, line + 1, strlen(line));
            }

            // Check if the line starts with a hashtag (#)
            if (line[0] == '#') {
                // Find the position of the first space (' ')
                char* space_position = strchr(line, ' ');

                // If space is found, remove everything up to the space
                if (space_position != NULL) {
                    memmove(line, space_position + 1, strlen(space_position));  // Remove everything up to the space
                } else {
                    line[0] = '\0';  // If no space is found, remove the entire line
                }
            }
            
            // Remove trailing newline character, if it exists
            size_t len = strlen(line);
            if (len > 0 && line[len - 1] == '\n') {
                line[len - 1] = '\0'; // Remove the newline
                len--;
            }

            // Check if the line (after removing '\n') ends with a colon
            if (len > 0 && line[len - 1] == ':') {
                // Write the line to the MipsScript.s file, including '\n'
                fprintf(fileMIPS, "%s\n", line);
            }
            // Check if "print = " is anywhere in the line
            char* print_position = strstr(line, "print = ");

            if (print_position != NULL && print_position == line) {
                // "print" is at the beginning of the line
                strncpy(print_value, print_position + 8, 3); // Copy only two characters after the equal sign
                print_value[3] = '\0'; // Null-terminate the string
                
                // Check for data type conditions
                if (strstr(line, "int") != NULL) {
                    fprintf(fileMIPS, "\tmove $a0, $%s\n", print_value);
                    fprintf(fileMIPS, "\tli $v0, 1\n");
                    fprintf(fileMIPS, "\tsyscall\n");
                } else if (strstr(line, "float") != NULL) {
                    // Handle float data type
                    fprintf(fileMIPS, "\tmove $a0, $%s\n", print_value);
                    fprintf(fileMIPS, "\tli $v0, 1\n");
                    fprintf(fileMIPS, "\tsyscall\n");
                } else if (strstr(line, "char") != NULL) {
                    // Handle char data type
                    fprintf(fileMIPS, "\tla $a0, %s\n", print_value);
                    fprintf(fileMIPS, "\tli $v0, 4\n");
                    fprintf(fileMIPS, "\tsyscall\n");
                } else {
                    // Handle other cases
                }
            }

            
            else if (strchr(line, '+') != NULL) {
                // '+' is in the line, split into three variables
                // Use var_name, var_value, and var_value2 as needed
                char* plus_position = strchr(line, '+');
                char* equal_position = strchr(line, '=');

                if (equal_position != NULL && plus_position != NULL) {
                    size_t split_position = equal_position - line;
                    size_t split_position2 = plus_position - (equal_position + 1);

                    strncpy(var_name, line, split_position);
                    var_name[split_position] = '\0'; // Null-terminate var_name

                    strncpy(var_value, equal_position + 1, split_position2);
                    var_value[split_position2] = '\0'; // Null-terminate var_value

                    strcpy(var_value2, plus_position + 1); // Copy the rest of the line

                    // Remove spaces from variable names
                    removeSpaces(var_name);
                    removeSpaces(var_value);
                    removeSpaces(var_value2);

                    fprintf(fileMIPS, "\tadd $%s, $%s, $%s\n", var_name, var_value, var_value2);
                } else {
                    // Handle the case when '+' or '=' is not found
                    fprintf(stderr, "Invalid line format: %s\n", line);
                }
            }

            // Check if '=' is in the line (variable assignment)
            else if (strchr(line, '=') != NULL) {
                // Split the line into two variables
                char* equal_position = strchr(line, '=');
                size_t split_position = equal_position - line;

                strncpy(var_name, line, split_position-1);
                var_name[split_position-1] = '\0'; // Null-terminate var_name

                strcpy(var_value, equal_position + 2); // Copy the rest of the line
                // Remove the last character of var_value
                var_value[strlen(var_value) - 1] = '\0';

                // Check if the line ends with a character that is not a number (var_value->type = character)
                if (!isdigit(var_value[strlen(var_value) - 1])) {
                    // write to filedata new asciiz variable
                    fprintf(fileDATA, "\tchar_%s: .asciiz \"%s\"\n", var_name, var_value);

                    printf("char %s\n", var_value);
                    fprintf(fileMIPS, "\tla $%s, char_%s\n", var_name, var_name);
                }
                // Check if the line has a '.' (dot) (var_value->type = float)
                else if (strchr(var_value, '.') != NULL) {
                    // write to filedata new float variable
                    printf("float %s\n", var_value);
                    fprintf(fileMIPS, "\tli $%s, %s\n", var_name, var_value);
                }
                // If t is second to last character, move temp register to temp register
                else if (var_value[strlen(var_value) - 2] == 't') {
                    fprintf(fileMIPS, "\tmove $%s, $%s\n", var_name, var_value);
                }
                // Else tr = int
                else{
                    fprintf(fileMIPS, "\tli $%s, %s\n", var_name, var_value);
                }
            }

            // Check if "Return To Caller" is in the line
            else if (strstr(line, "Return To Caller") != NULL) {
                fprintf(fileMIPS, "\tjr $ra\n");
            }
            // Check if "Jump" is at the start of the line
            else if (strstr(line, "Jump ") == line) {
                char* jump_position = line + 5; // Move to the character after "Jump "
                fprintf(fileMIPS, "\tjal %s\n", jump_position);
            }

        }

        fprintf(fileMIPS, "\tli $v0, 10\n\tsyscall\n");

        printf(ANSI_COLOR_ORANGE "\n\n #####- Done Converting IR Code to MIPS -##### \n" ANSI_COLOR_RESET);

        fclose(fileIR);
        fclose(fileMIPS);
        fclose(fileDATA);
    }

    void removeSpaces(char* str) {
        int i, j = 0;
        for (i = 0; str[i]; i++) {
            if (str[i] != ' ') {
                str[j++] = str[i];
            }
        }
        str[j] = '\0';
    }


#endif