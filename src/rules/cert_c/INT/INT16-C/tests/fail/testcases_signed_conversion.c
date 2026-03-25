/*
 * Rule: INT16-C
 * Source: testcases
 * Status: FAIL - Bitwise operations on signed integers
 */

/* Signed int used with bitwise AND */
void signed_bitwise_and(void) {
    int value = 42;
    int result = value & 0xFF;
    (void)result;
}

/* Signed int used with bitwise OR */
void signed_bitwise_or(void) {
    int flags = 0;
    flags = flags | 0x01;
    (void)flags;
}

/* Signed int used with left shift */
void signed_left_shift(void) {
    int value = 1;
    int shifted = value << 4;
    (void)shifted;
}

/* Signed int used with bitwise XOR */
void signed_bitwise_xor(void) {
    int a = 0xAA;
    int toggled = a ^ 0xFF;
    (void)toggled;
}

/* Signed int used with right shift */
void signed_right_shift(int input) {
    int result = input >> 2;
    (void)result;
}
