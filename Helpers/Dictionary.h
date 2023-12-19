
#ifndef DICTIONARY_H
#define DICTIONARY_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <limits.h>

    #define MAX_ENTRIES 100

    struct Dictionary myDict;

    // Define a structure for key-value pairs
    struct KeyValuePair {
        char key[50];
        int value;
    };

    // Define a structure for the dictionary
    struct Dictionary {
        struct KeyValuePair entries[MAX_ENTRIES];
        int count;
    };

    // Function to initialize the dictionary
    void initDictionary(struct Dictionary* dict) {
        dict->count = 0;
    }

    // Function to add a key-value pair to the dictionary
    void addToDictionary(struct Dictionary* dict, char* key, int value) {
        if (dict->count < MAX_ENTRIES) {
            strcpy(dict->entries[dict->count].key, key);
            dict->entries[dict->count].value = value;
            dict->count++;
        } else {
            printf("Dictionary is full. Cannot add more entries.\n");
        }
    }

    // Function to get the value associated with a key
    int getFromDictionary(struct Dictionary* dict, char* key) {
        for (int i = 0; i < dict->count; i++) {
            if (strcmp(dict->entries[i].key, key) == 0) {
                return dict->entries[i].value;
            }
        }
        return INT_MIN; // Key not found
    }

    // Function to clear the dictionary
    void clearDictionary(struct Dictionary* dict) {
        dict->count = 0; // Reset the count to zero
    }


#endif
