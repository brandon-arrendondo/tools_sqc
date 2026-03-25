/* Rule: INT08-C
 * Source: testcases
 * Status: FAIL - Arithmetic on narrow integer types without overflow protection
 */

/* Case 1: short addition without overflow check */
void test_short_add(void) {
    short a = 32000;
    short b = 1000;
    short result = a + b;
}

/* Case 2: unsigned char multiplication without check */
void test_uchar_multiply(void) {
    unsigned char x = 200;
    unsigned char y = 2;
    unsigned char result = x * y;
}

/* Case 3: signed char subtraction */
void test_schar_subtract(void) {
    char c = -100;
    char d = 50;
    char result = c - d;
}

/* Case 4: short shift operation without protection */
void test_short_shift(void) {
    short val = 16000;
    short shifted = val << 2;
}
