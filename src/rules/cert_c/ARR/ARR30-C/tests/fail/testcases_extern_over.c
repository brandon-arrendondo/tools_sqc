/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: External array accessed beyond its declared bounds
 */

#include <stdio.h>

extern int extern_array[8];  // External declaration

int extern_array[8] = {10, 20, 30, 40, 50, 60, 70, 80};  // Definition

void test_extern() {
    // Access beyond extern array bounds
    printf("extern_array[10] = %d\n", extern_array[10]);  // Line 14 - VIOLATION
    extern_array[12] = 999;  // Line 15 - VIOLATION
}

int main(void) {
    test_extern();
    printf("extern_array[15] = %d\n", extern_array[15]);  // Line 19 - VIOLATION
    return 0;
}
