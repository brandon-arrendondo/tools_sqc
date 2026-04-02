/*
 * Rule: MSC13-C
 * Source: testcases
 * Status: PASS - Compound assignment reads the LHS, so init is not dead
 */

#include <stdio.h>

void test_plus_equals(void) {
    int x = 10;
    x += 5;
    printf("%d", x);
}

void test_minus_equals(void) {
    int total = 100;
    total -= 20;
    printf("%d", total);
}

void test_multiply_equals(void) {
    int val = 3;
    val *= 7;
    printf("%d", val);
}

void test_divide_equals(void) {
    int num = 42;
    num /= 6;
    printf("%d", num);
}

void test_modulo_equals(void) {
    int rem = 17;
    rem %= 5;
    printf("%d", rem);
}

void test_bitwise_or_equals(void) {
    int flags = 0x01;
    flags |= 0x10;
    printf("%d", flags);
}

void test_shift_left_equals(void) {
    int bits = 1;
    bits <<= 4;
    printf("%d", bits);
}
