/*
 * Rule: MSC13-C
 * Status: PASS - Variable is used in return statement
 */

int f(void) {
    int x = 42;
    return x;
}
