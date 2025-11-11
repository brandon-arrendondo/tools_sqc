/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Off-by-one error - forgot to account for null terminator in size calculation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char source[] = "Hello World";
    char *dest = malloc(strlen(source));  // Missing +1 for null terminator

    strcpy(dest, source);  // Overwrites one byte beyond allocated memory
    printf("Copied: %s\n", dest);
    free(dest);

    return 0;
}