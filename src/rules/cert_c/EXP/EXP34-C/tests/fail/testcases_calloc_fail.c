/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Using calloc result without checking for NULL
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *buffer = calloc(100, sizeof(char));

    // Not checking if calloc succeeded
    strcpy(buffer, "Hello World");
    printf("Buffer: %s\n", buffer);

    free(buffer);
    return 0;
}