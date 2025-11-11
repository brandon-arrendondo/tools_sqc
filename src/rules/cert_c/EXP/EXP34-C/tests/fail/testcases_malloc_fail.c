/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: FAIL
 * Reason: Using malloc result without checking for NULL
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = malloc(sizeof(int));

    // Not checking if malloc succeeded
    *ptr = 42;
    printf("Value: %d\n", *ptr);

    free(ptr);
    return 0;
}