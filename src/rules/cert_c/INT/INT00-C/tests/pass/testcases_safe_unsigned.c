/*
 * Rule: INT00-C
 * Source: testcases
 * Status: PASS - Safe unsigned patterns
 */

/* Both operands unsigned, subtraction guarded */
unsigned int safe_subtract(unsigned int a, unsigned int b) {
    if (a >= b) {
        return a - b;
    }
    return 0;
}

/* Same-type comparison */
int same_type_compare(unsigned int a, unsigned int b) {
    return a < b;
}
