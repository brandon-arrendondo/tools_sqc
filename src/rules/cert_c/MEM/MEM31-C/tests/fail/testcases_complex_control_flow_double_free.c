/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

#include <stdio.h>
#include <stdlib.h>

int process_with_goto(int condition) {
    int *buffer1 = malloc(100 * sizeof(int));
    int *buffer2 = malloc(200 * sizeof(int));

    if (!buffer1 || !buffer2) {
        goto cleanup;
    }

    if (condition == 1) {
        free(buffer1);
        goto cleanup;  // Will free buffer1 again
    }

    if (condition == 2) {
        free(buffer2);
        goto cleanup;  // Will free buffer2 again
    }

    // Normal processing
    printf("Processing data\n");

cleanup:
    if (buffer1) free(buffer1);  // Potential double free
    if (buffer2) free(buffer2);  // Potential double free

    return 0;
}

int main() {
    process_with_goto(0);  // Normal case
    process_with_goto(1);  // Double free buffer1
    process_with_goto(2);  // Double free buffer2

    return 0;
}