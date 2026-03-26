/*
 * Rule: MSC12-C
 * Source: wiki
 * Status: FAIL - Should trigger MSC12-C violation
 * Pattern: Pointer dereference with no effect
 */

void func(void) {
    int arr[10] = {0};
    int *p = arr;
    *p++;  /* Noncompliant: dereference value discarded, only pointer advanced */
}
