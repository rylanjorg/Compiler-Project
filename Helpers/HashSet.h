#ifndef HASHSET_H
#define HASHSET_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <string.h>

    #define INITIAL_SIZE 100
    #define LOAD_FACTOR_THRESHOLD 0.75

    // Node structure for linked list
    struct Node {
        char key[50];
        struct Node* next;
    };

    // HashSet structure
    struct HashSet {
        struct Node** buckets;
        int size;
        int count;  // Number of elements in the hash set
    };

    // Function to create a new HashSet
    struct HashSet* createHashSet() {
        struct HashSet* set = (struct HashSet*)malloc(sizeof(struct HashSet));
        if (set == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            exit(EXIT_FAILURE);
        }

        set->size = INITIAL_SIZE;
        set->count = 0;

        set->buckets = (struct Node**)calloc(set->size, sizeof(struct Node*));
        if (set->buckets == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            exit(EXIT_FAILURE);
        }

        return set;
    }

    // Function to hash a string
    unsigned int hash(char* key, int size) {
        unsigned int hashVal = 0;
        while (*key != '\0') {
            hashVal = (hashVal * 31 + *key) % size;
            ++key;
        }
        return hashVal;
    }

    // Function to check if a key is present in the HashSet
    bool contains(struct HashSet* set, char* key) {
        unsigned int index = hash(key, set->size);
        struct Node* current = set->buckets[index];

        while (current != NULL) {
            if (strcmp(current->key, key) == 0) {
                return true;  // Key found
            }
            current = current->next;
        }

        return false;  // Key not found
    }

    // Function to resize the HashSet if needed
    void resizeHashSet(struct HashSet* set) {
        if ((double)set->count / set->size > LOAD_FACTOR_THRESHOLD) {
            int newSize = set->size * 2;
            struct Node** newBuckets = (struct Node**)calloc(newSize, sizeof(struct Node*));
            if (newBuckets == NULL) {
                fprintf(stderr, "Memory allocation error\n");
                exit(EXIT_FAILURE);
            }

            // Rehash existing elements
            for (int i = 0; i < set->size; ++i) {
                struct Node* current = set->buckets[i];
                while (current != NULL) {
                    unsigned int newIndex = hash(current->key, newSize);
                    struct Node* temp = current;
                    current = current->next;
                    temp->next = newBuckets[newIndex];
                    newBuckets[newIndex] = temp;
                }
            }

            free(set->buckets);
            set->buckets = newBuckets;
            set->size = newSize;
        }
    }

    // Function to add a key to the HashSet
    void addToHashSet(struct HashSet* set, char* key) {
        if (!contains(set, key)) {
            unsigned int index = hash(key, set->size);
            struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
            if (newNode == NULL) {
                fprintf(stderr, "Memory allocation error\n");
                exit(EXIT_FAILURE);
            }

            strcpy(newNode->key, key);
            newNode->next = set->buckets[index];
            set->buckets[index] = newNode;
            set->count++;

            // Resize the HashSet if needed
            resizeHashSet(set);
        }
    }

    // Function to free resources used by the HashSet
    void freeHashSet(struct HashSet* set) {
        for (int i = 0; i < set->size; ++i) {
            struct Node* current = set->buckets[i];
            while (current != NULL) {
                struct Node* temp = current;
                current = current->next;
                free(temp);
            }
        }

        free(set->buckets);
        free(set);
    }

#endif
