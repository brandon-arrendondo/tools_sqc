/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Using sizeof with atomic type for pointer arithmetic
 */

#include <stdatomic.h>

void atomic_sizeof(void) {
    _Atomic int atomic_array[50];
    _Atomic int *ptr = atomic_array;
    int position = 15;

    // Scaling by sizeof(_Atomic int)
    _Atomic int *target = ptr + (position * sizeof(_Atomic int));  // Line 14 - VIOLATION
    atomic_store(target, 42);
}

int main(void) {
    atomic_sizeof();
    return 0;
}
