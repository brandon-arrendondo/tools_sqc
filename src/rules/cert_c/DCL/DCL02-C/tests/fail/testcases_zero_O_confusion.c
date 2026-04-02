/*
 * Rule: DCL02-C
 * Status: FAIL - Identifiers differ only by 0 (zero) vs O (letter)
 */

void f(void) {
    int id_O = 1;
    int id_0 = 2;  /* VIOLATION: id_O vs id_0 are visually similar */
}
