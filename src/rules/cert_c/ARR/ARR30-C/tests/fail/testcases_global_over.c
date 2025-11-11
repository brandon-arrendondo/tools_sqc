/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Global array accessed beyond its declared bounds
 */

#include <stdio.h>

int global_array[8] = {10, 20, 30, 40, 50, 60, 70, 80};

void access_global() {
    // Access beyond global array bounds
    printf("global_array[10] = %d\n", global_array[10]);
    global_array[12] = 999;
}

int main(void) {
    access_global();

    // Direct out-of-bounds access
    printf("global_array[15] = %d\n", global_array[15]);

    return 0;
}