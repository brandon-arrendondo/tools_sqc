/* Rule: FLP05-C
 * Source: testcases
 * Status: FAIL - Float operations with denormalized constants
 */

#include <stdio.h>

/* Case 1: Float multiplied by denormalized constant */
void test_multiply_denorm(void) {
    float x = 0.5f;
    float result = x * 7e-45;
}

/* Case 2: Float divided by denormalized constant */
void test_divide_denorm(void) {
    float y = 1.0f;
    float result = y / 1e-40;
}

/* Case 3: Float initialized with denormalized value */
void test_init_denorm(void) {
    float tiny = 1e-45;
    float small = 7e-46;
}

/* Case 4: Float variable in arithmetic with very small constant */
void test_arithmetic_denorm(void) {
    float val = 0.333f;
    float scaled = val * 5e-39;
}
