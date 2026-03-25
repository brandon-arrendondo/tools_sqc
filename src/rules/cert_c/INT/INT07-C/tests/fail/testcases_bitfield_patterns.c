/*
 * Rule: INT07-C
 * Source: testcases
 * Status: FAIL - Plain char used in numeric/bitfield contexts
 */

/* Plain char used in multiplication */
void char_multiply(void) {
    char c = 128;
    int result = c * 2;
    (void)result;
}

/* Plain char used in division */
void char_division(void) {
    char c = 200;
    int i = 1000;
    int result = i / c;
    (void)result;
}

/* Plain char assigned a numeric literal */
void char_numeric_assign(void) {
    char c;
    c = 200;
}

/* Plain char in modulo operation */
void char_modulo(void) {
    char c = 50;
    int rem = c % 10;
    (void)rem;
}

/* Plain char parameter used in subtraction */
void char_subtract(char val) {
    int result = val - 48;
    (void)result;
}
