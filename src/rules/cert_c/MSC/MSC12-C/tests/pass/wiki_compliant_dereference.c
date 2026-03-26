/*
 * Rule: MSC12-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MSC12-C violation
 * Pattern: Correct pointer operations that have effect
 */

void func(void) {
    int arr[10] = {0};
    int *p = arr;

    /* Compliant: advance pointer without wasted dereference */
    ++p;

    /* Compliant: increment dereferenced value */
    (*p)++;
}
