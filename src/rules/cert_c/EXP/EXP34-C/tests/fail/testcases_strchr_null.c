/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Using strchr result without checking for NULL
 */

#include <stdio.h>
#include <string.h>

int main() {
    char str[] = "Hello World";
    char *found = strchr(str, 'X');  // Character not in string

    // strchr returns NULL when character not found
    printf("Found char: %c\n", *found);

    return 0;
}