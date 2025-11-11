/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: PASS
 * Reason: Pointer arithmetic on variable length array
 */

#include <stdio.h>

void vla_operations(int n) {
    int vla[n];
    int *ptr = vla;

    // Initialize VLA with pointer arithmetic - COMPLIANT
    for (int i = 0; i < n; i++) {
        *(ptr + i) = i * 3;
    }

    // Access VLA elements - COMPLIANT
    printf("vla[%d] = %d\n", n/2, vla[n/2]);

    // Pointer increment within VLA bounds - COMPLIANT
    ptr += n/2;
    printf("Middle element: %d\n", *ptr);
}

int main(void) {
    vla_operations(20);
    return 0;
}
