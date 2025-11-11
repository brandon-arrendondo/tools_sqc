/*
 * Rule: EXP34-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP34-C violation
 */

/*
 * Rule: EXP34-C - Do not dereference null pointers
 * Status: PASS
 * Reason: Conditional operator ensures pointer is valid before dereference
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr1 = malloc(sizeof(int));
    int *ptr2 = NULL;

    if (ptr1 != NULL) {
        *ptr1 = 100;
    }

    // Safe use of conditional operator
    int value1 = (ptr1 != NULL) ? *ptr1 : 0;
    int value2 = (ptr2 != NULL) ? *ptr2 : -1;

    printf("Value1: %d\n", value1);
    printf("Value2: %d\n", value2);

    if (ptr1 != NULL) {
        free(ptr1);
    }

    return 0;
}