/*
 * Rule: MSC12-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MSC12-C violation
 * Pattern: Volatile pointer dereference cast to void
 */

void func(void) {
    volatile int data[10] = {0};
    volatile int *p = data;
    (void) *(p++);  /* Compliant: cast to void is intentional */
}
