/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Comparing pointers to different atomic arrays
 */

#include <stdatomic.h>

_Atomic int atomic_array1[10];
_Atomic int atomic_array2[10];

void atomic_compare(void) {
    _Atomic int *ptr1 = atomic_array1;
    _Atomic int *ptr2 = atomic_array2;

    // Compare pointers from different atomic arrays
    if (ptr1 <= ptr2) {  // Line 16 - VIOLATION
        // Undefined behavior
    }
}

int main(void) {
    atomic_compare();
    return 0;
}
