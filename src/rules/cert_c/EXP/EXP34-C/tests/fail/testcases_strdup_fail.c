/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Using strdup result without checking for NULL
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char *original = "Hello World";
    char *copy = strdup(original);

    // Not checking if strdup succeeded
    printf("Copy: %s\n", copy);
    copy[0] = 'h';  // Modifying without NULL check

    free(copy);
    return 0;
}