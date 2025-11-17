/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Subtracting pointers from different VLAs
 */

#include <stddef.h>

void vla_subtract(int n) {
    int vla1[n];
    int vla2[n];

    int *ptr1 = vla1;
    int *ptr2 = vla2;

    // Subtract pointers from different VLAs
    ptrdiff_t diff = ptr2 - ptr1;  // Line 17 - VIOLATION
}

int main(void) {
    vla_subtract(10);
    return 0;
}
