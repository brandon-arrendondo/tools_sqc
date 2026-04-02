/*
 * Rule: DCL02-C
 * Status: FAIL - Identifiers differ by rn vs m (visually similar in some fonts)
 */

void f(void) {
    int rn_value = 1;
    int m_value = 2;  /* VIOLATION: rn vs m visually similar */
}
