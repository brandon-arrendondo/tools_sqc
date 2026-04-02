/*
 * Rule: DCL02-C
 * Status: FAIL - Identifiers differ by B vs 8
 */

void f(void) {
    int sumB = 1;
    int sum8 = 2;  /* VIOLATION: B vs 8 visually similar */
}
