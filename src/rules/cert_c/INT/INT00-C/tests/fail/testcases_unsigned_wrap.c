/*
 * Rule: INT00-C
 * Source: testcases
 * Status: FAIL - Unsigned subtraction without guard
 */

/* Unsigned wrap on subtraction */
void unsigned_subtract(unsigned int a, unsigned int b) {
    unsigned int result = a - b;
    (void)result;
}
