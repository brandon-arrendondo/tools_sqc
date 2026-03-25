/*
 * Rule: INT07-C
 * Source: testcases
 * Status: FAIL - Plain char used in arithmetic/numeric contexts
 */

/* Char used in arithmetic */
void char_arithmetic(char c) {
    int result = c + 1;
    (void)result;
}

/* Char used as array index */
void char_array_index(char c) {
    int table[256];
    int val = table[c];
    (void)val;
}

/* Char in comparison with int */
void char_comparison(char c) {
    if (c > 127) {
        return;
    }
}
