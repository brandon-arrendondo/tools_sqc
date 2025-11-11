/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Type casting used to bypass array bounds restrictions
 */

#include <stdio.h>

int main(void) {
    char small_array[4] = {1, 2, 3, 4};

    // Cast to int* to access as larger type beyond bounds
    int *int_ptr = (int *)small_array;

    // This may access beyond the 4-byte array
    printf("int_ptr[1] = %d\n", int_ptr[1]);
    int_ptr[2] = 0x12345678;

    return 0;
}