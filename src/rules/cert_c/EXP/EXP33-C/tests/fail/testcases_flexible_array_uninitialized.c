/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: flexible_array_uninitialized.c
 */

#include <stdio.h>
#include <stdlib.h>

struct FlexArray {
    int count;
    int data[];  /* Flexible array member */
};

/* NON-COMPLIANT: Flexible array member uninitialized */
void unsafe_flexible_array(void) {
    struct FlexArray *arr = malloc(sizeof(struct FlexArray) + 5 * sizeof(int));
    if (!arr) return;

    arr->count = 5;
    /* arr->data[] remains uninitialized */

    for (int i = 0; i < arr->count; i++) {
        printf("data[%d] = %d\n", i, arr->data[i]);  /* Reading uninitialized */
    }

    free(arr);
}

int main(void) {
    unsafe_flexible_array();
    return 0;
}