/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Accessing pointer after double free
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;

    free(ptr);
    free(ptr);  // Double free

    // Accessing after double free
    printf("Value: %d\n", *ptr);

    return 0;
}