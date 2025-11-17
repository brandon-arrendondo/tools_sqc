/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Using strtok result without checking for NULL
 */

#include <stdio.h>
#include <string.h>

int main() {
    char str[] = "no,delimiters,here";
    char *token = strtok(str, ";");  // Wrong delimiter

    // strtok returns NULL when no more tokens
    printf("Token: %s\n", token);  // May be NULL
    printf("First char: %c\n", token[0]);  // Dereferencing potentially NULL

    return 0;
}