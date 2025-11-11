/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

int process_data(int condition) {
    int *buffer = malloc(100 * sizeof(int));

    if (buffer == NULL) {
        return -1;
    }

    // Some processing
    for (int i = 0; i < 100; i++) {
        buffer[i] = i;
    }

    // Conditional free
    if (condition > 0) {
        free(buffer);
    }

    // Always free - potential double free
    free(buffer);  // Double free if condition > 0

    return 0;
}

int main() {
    process_data(1);   // Will cause double free
    process_data(-1);  // Normal case

    return 0;
}