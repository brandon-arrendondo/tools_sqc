/*
 * Rule: DCL02-C
 * Status: FAIL - Identifiers differ by S vs 5
 */

void f(void) {
    int valS = 1;
    int val5 = 2;  /* VIOLATION: S vs 5 visually similar */
}
