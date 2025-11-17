/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: PASS
 * Reason: Pointer is checked for NULL before dereferencing
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = malloc(sizeof(int));

    if (ptr != NULL) {
        *ptr = 42;
        printf("Value: %d\n", *ptr);
        free(ptr);
    } else {
        printf("Memory allocation failed\n");
    }

    return 0;
}