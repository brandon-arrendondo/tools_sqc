/*
 * Rule: INT16-C
 * Source: testcases
 * Status: PASS - Validated signed-to-unsigned conversion
 */

/* Checked before conversion */
unsigned int safe_convert(int x) {
    if (x >= 0) {
        return (unsigned int)x;
    }
    return 0;
}

/* Unsigned-to-unsigned — no issue */
unsigned long widen(unsigned int x) {
    return x;
}
