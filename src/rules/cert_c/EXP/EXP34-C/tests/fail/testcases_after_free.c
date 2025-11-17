/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Using pointer after it has been freed (use after free)
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;

    free(ptr);

    // Using pointer after free (undefined behavior, often NULL)
    printf("Value after free: %d\n", *ptr);

    return 0;
}