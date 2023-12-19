#ifndef CONDITIONAL_LABEL_COUNTER_H
#define CONDITIONAL_LABEL_COUNTER_H

    #include <stdio.h>
    #include <stdlib.h>
    
    int BRANCH_COUNTER = 0;
    char BRANCH_LABEL = 'a';

    // Function to generate a combined label
    char* generateNewBranchLabel(int incrementLetter) {
        char* label = (char*)malloc(3);  // Assuming a single letter and two digits
        
        // Combine the letter and number into the label
        snprintf(label, 3, "%c%d", BRANCH_LABEL, BRANCH_COUNTER);
        
        // Increment the letter if requested
        if (incrementLetter == 1) {
            if (BRANCH_LABEL < 'z') {
                BRANCH_LABEL++;
            } else {
                // Reset to 'a' if 'z' is reached
                BRANCH_LABEL = 'a';
            }
        }
        
        // Increment the number
        BRANCH_COUNTER++;
        
        return label;
    }

      // Function to generate a combined label
    char* lastBranchLabel() {
        char* label = (char*)malloc(3);  // Assuming a single letter and two digits

        // Combine the letter and number into the label
        snprintf(label, 3, "%c%d", BRANCH_LABEL, BRANCH_COUNTER-1);
       
        return label;
    }

    void IncrementBranchLabel()
    {
        if (BRANCH_LABEL < 'z') {
            BRANCH_LABEL++;
        } else {
            // Reset to 'a' if 'z' is reached
            BRANCH_LABEL = 'a';
        }

        BRANCH_COUNTER = 0;
    }


#endif