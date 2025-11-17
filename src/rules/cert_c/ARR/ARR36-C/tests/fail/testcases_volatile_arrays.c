/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Subtracting pointers to different volatile arrays
 */

#include <stddef.h>

volatile int vol_array1[10];
volatile int vol_array2[10];

void volatile_subtract(void) {
    volatile int *ptr1 = vol_array1;
    volatile int *ptr2 = vol_array2;

    // Subtract pointers from different volatile arrays
    ptrdiff_t diff = ptr2 - ptr1;  // Line 17 - VIOLATION
}

int main(void) {
    volatile_subtract();
    return 0;
}
