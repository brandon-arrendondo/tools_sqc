/*
 * Rule: DCL02-C
 * Status: FAIL - Identifiers differ only by 1 (one) vs l (lowercase L)
 */

void f(void) {
    int var1 = 1;
    int varl = 2;  /* VIOLATION: var1 vs varl visually similar */
}
