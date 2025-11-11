/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Variable Length Array (VLA) accessed beyond runtime-determined bounds
 */

#include <stdio.h>

void process_vla(int n) {
    int vla[n];

    // Initialize VLA
    for (int i = 0; i < n; i++) {
        vla[i] = i * 10;
    }

    // Access beyond VLA bounds
    printf("vla[%d] = %d\n", n + 5, vla[n + 5]);
    vla[n + 10] = 999;
}

int main(void) {
    process_vla(5);
    return 0;
}