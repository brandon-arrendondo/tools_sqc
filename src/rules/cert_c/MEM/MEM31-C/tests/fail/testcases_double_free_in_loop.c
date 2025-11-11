/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

#define NUM_POINTERS 5

int main() {
    int *pointers[NUM_POINTERS];

    // Allocate memory
    for (int i = 0; i < NUM_POINTERS; i++) {
        pointers[i] = malloc(10 * sizeof(int));
    }

    // Free memory in loop - potential double free
    for (int i = 0; i < NUM_POINTERS; i++) {
        free(pointers[i]);
    }

    // Double free in second loop
    for (int i = 0; i < NUM_POINTERS; i++) {
        free(pointers[i]);  // Double free violation
    }

    return 0;
}