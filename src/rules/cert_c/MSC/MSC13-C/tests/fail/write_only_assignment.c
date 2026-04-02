/*
 * Rule: MSC13-C
 * Status: FAIL - Variable assigned via = but never read
 */

void f(int input) {
    int result = 0;    /* VIOLATION: result is only written, never read */
    result = input + 1;
}
