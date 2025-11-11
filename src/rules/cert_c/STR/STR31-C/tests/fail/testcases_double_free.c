/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Reusing freed memory for string operations leads to undefined behavior
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *buffer = malloc(20);

    strcpy(buffer, "Initial");
    free(buffer);

    // Using freed memory - undefined behavior
    strcpy(buffer, "After free");  // Writing to freed memory
    printf("Result: %s\n", buffer);

    return 0;
}