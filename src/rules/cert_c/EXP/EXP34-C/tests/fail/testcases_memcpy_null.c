/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Using memcpy with NULL source or destination
 */

#include <stdio.h>
#include <string.h>

int main() {
    char *src = NULL;
    char dest[10];

    // Using memcpy with NULL source
    memcpy(dest, src, 5);
    printf("Dest: %s\n", dest);

    return 0;
}