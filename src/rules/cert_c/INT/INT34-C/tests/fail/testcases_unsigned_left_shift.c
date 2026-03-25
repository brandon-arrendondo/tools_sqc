/*
 * Rule: INT34-C
 * Source: testcases
 * Status: FAIL - Unsigned left shifts without validation
 */

/* Unsigned left shift without validation */
unsigned int unsigned_left_no_check(unsigned int x, unsigned int amount) {
    return x << amount;
}

/* Unsigned left shift in nested expression */
unsigned int unsigned_left_nested(unsigned int x, unsigned int a, unsigned int b) {
    return (x << a) | (x << b);
}
