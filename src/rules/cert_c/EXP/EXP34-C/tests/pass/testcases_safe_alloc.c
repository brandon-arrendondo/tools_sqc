/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: PASS
 * Reason: Memory allocation is checked before use and properly handled
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    char *buffer = calloc(100, sizeof(char));

    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    strcpy(buffer, "Hello, World!");
    printf("Buffer contains: %s\n", buffer);

    free(buffer);
    buffer = NULL;  // Prevent accidental reuse

    return 0;
}