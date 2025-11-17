/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Using realloc result without checking for NULL
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = malloc(10 * sizeof(int));

    // realloc can fail and return NULL
    ptr = realloc(ptr, 1000000 * sizeof(int));

    // Using result without checking for NULL
    ptr[0] = 42;
    printf("Value: %d\n", ptr[0]);

    free(ptr);
    return 0;
}