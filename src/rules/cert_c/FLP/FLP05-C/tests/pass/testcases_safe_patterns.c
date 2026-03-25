/* Rule: FLP05-C
 * Source: testcases
 * Status: PASS - No denormalized float operations
 */

#include <stdio.h>

/* Case 1: Double used for very small values (safe - wider range) */
void test_double_small_values(void) {
    double x = 1.0 / 3.0;
    double result = x * 7e-45;
}

/* Case 2: Float with normal-range values only */
void test_float_normal_range(void) {
    float a = 1.0f;
    float b = a * 0.5f;
    float c = b / 2.0f;
}

/* Case 3: Integer arithmetic (no float denorm issue) */
void test_integer_ops(void) {
    int x = 100;
    int y = x * 3;
    int z = y / 7;
}

/* Case 4: Float with normal small values (above denorm threshold) */
void test_normal_small_float(void) {
    float val = 0.333f;
    float result = val * 1e-10f;
}
