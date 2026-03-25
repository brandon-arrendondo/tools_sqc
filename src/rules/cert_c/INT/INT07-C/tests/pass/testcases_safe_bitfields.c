/*
 * Rule: INT07-C
 * Source: testcases
 * Status: PASS - Explicitly signed/unsigned char used for numeric values
 */

/* Unsigned char in arithmetic is compliant */
void unsigned_char_arithmetic(void) {
    unsigned char c = 200;
    int result = c * 2;
    (void)result;
}

/* Signed char in arithmetic is compliant */
void signed_char_arithmetic(void) {
    signed char c = 100;
    int result = c + 1;
    (void)result;
}

/* Plain char pointer is not flagged (INT07-C is about values) */
void char_pointer(void) {
    char *str = "hello";
    (void)str;
}

/* Plain char array is not flagged */
void char_array(void) {
    char buf[64];
    buf[0] = 'A';
    (void)buf;
}

/* Int arithmetic has nothing to do with char */
void int_arithmetic(void) {
    int a = 200;
    int b = a * 2;
    (void)b;
}
