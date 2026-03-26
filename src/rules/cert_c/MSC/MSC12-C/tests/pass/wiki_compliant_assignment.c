/*
 * Rule: MSC12-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MSC12-C violation
 * Pattern: Proper assignment instead of comparison
 */

void func(void) {
    int a = 1;
    int b = 2;
    a = b;  /* Compliant: assignment has effect */
}
