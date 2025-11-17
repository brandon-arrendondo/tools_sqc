/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Static array accessed beyond declared bounds
 */

#include <stdio.h>

static int static_array[8] = {10, 20, 30, 40, 50, 60, 70, 80};

static void access_static() {
    // Access beyond static array bounds
    printf("static_array[10] = %d\n", static_array[10]);  // Line 13 - VIOLATION
    static_array[12] = 999;  // Line 14 - VIOLATION
}

int main(void) {
    access_static();
    printf("static_array[15] = %d\n", static_array[15]);  // Line 19 - VIOLATION
    return 0;
}
