/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: PASS
 * Reason: Array elements are freed before the array itself, proper cleanup order
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
    char **strings = malloc(3 * sizeof(char*));
    if (strings == NULL) {
        return -1;
    }

    // Allocate individual strings
    for (int i = 0; i < 3; i++) {
        strings[i] = malloc(20);
        if (strings[i] == NULL) {
            // Free previously allocated strings
            for (int j = 0; j < i; j++) {
                free(strings[j]);
            }
            free(strings);
            return -1;
        }
        snprintf(strings[i], 20, "String %d", i);
    }

    // Use the strings
    for (int i = 0; i < 3; i++) {
        printf("%s\n", strings[i]);
    }

    // Proper cleanup - free elements first
    for (int i = 0; i < 3; i++) {
        free(strings[i]);
        strings[i] = NULL;
    }
    free(strings);
    strings = NULL;

    return 0;
}