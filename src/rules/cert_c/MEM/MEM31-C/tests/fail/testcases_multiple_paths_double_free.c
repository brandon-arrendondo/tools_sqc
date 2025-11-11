/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

void cleanup_function(int **ptr) {
    if (ptr && *ptr) {
        free(*ptr);
        // Missing: *ptr = NULL
    }
}

int main() {
    int *data = malloc(50 * sizeof(int));

    if (data) {
        // Use data
        for (int i = 0; i < 50; i++) {
            data[i] = i;
        }

        // First cleanup call
        cleanup_function(&data);

        // Second cleanup call - double free
        cleanup_function(&data);
    }

    return 0;
}