/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on single atomic variable
 */

#include <stdatomic.h>

void atomic_test(void) {
    _Atomic int atomic_var = 0;
    _Atomic int *ptr = &atomic_var;

    // Pointer arithmetic on single atomic variable
    ptr++;  // Line 14 - VIOLATION
    atomic_store(ptr, 100);  // Undefined behavior
}

int main(void) {
    atomic_test();
    return 0;
}
