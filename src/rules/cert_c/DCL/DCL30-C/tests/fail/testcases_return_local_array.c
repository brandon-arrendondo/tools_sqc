/*
 * Rule: DCL30-C
 * Status: FAIL - Returning address of local array
 */

int *f(void) {
    int arr[10];
    return arr;  /* VIOLATION: returns address of local array */
}
