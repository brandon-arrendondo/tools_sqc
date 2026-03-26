/*
 * Rule: MSC12-C
 * Source: wiki
 * Status: FAIL - Should trigger MSC12-C violation
 * Pattern: Comparison used as statement (no effect)
 */

void func(void) {
    int a = 1;
    int b = 2;
    a == b;  /* Noncompliant: comparison result discarded */
}
