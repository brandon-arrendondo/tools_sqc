/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: PASS
 * Reason: Using array length constant for pointer arithmetic
 */

#include <stdio.h>

#define ARRAY_SIZE 100

void iterate_array(void) {
    double data[ARRAY_SIZE];
    double *ptr = data;

    // Use array size constant, not sizeof - COMPLIANT
    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        *(ptr + i) = i * 1.5;
    }

    printf("Array filled\n");
}

int main(void) {
    iterate_array();
    return 0;
}
