/*
 * Rule: ARR37-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR37-C violation
 */

/*
 * Rule: ARR37-C - Do not add or subtract an integer to a pointer to a non-array object
 * Status: FAIL
 * Reason: Pointer arithmetic on single thread-local variable
 */

#include <threads.h>

thread_local int tls_var = 0;

void thread_local_test(void) {
    int *ptr = &tls_var;

    // Pointer arithmetic on thread-local single variable
    ptr++;  // Line 15 - VIOLATION
    *ptr = 999;  // Undefined behavior
}

int main(void) {
    thread_local_test();
    return 0;
}
