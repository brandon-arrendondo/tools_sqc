/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using sizeof() as loop bound with pointer arithmetic
 */

#include <stdio.h>

void iterate_array(void) {
    double data[50];
    double *ptr = data;

    // sizeof(data) returns bytes, pointer arithmetic scales again
    for (size_t i = 0; i < sizeof(data); i++) {
        *(ptr + i) = i * 1.5;  // Line 14 - VIOLATION (ptr+i uses scaled sizeof)
    }
}

int main(void) {
    iterate_array();
    return 0;
}
