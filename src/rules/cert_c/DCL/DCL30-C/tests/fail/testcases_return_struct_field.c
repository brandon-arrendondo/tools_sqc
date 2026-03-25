/*
 * Rule: DCL30-C
 * Source: testcases
 * Status: FAIL - Returning pointer to local via struct or global assignment
 */

#include <stdlib.h>

/* Returning address of local array */
int *return_local_array(void) {
    int arr[10];
    arr[0] = 42;
    return arr;
}

/* Returning address of local variable */
int *return_local_var(void) {
    int x = 42;
    return &x;
}
