/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Comparing pointers to thread-local arrays (different storage)
 */

#include <threads.h>

thread_local int tls_array1[10];
thread_local int tls_array2[10];

void compare_tls(void) {
    int *ptr1 = tls_array1;
    int *ptr2 = tls_array2;

    // Compare pointers from different thread-local arrays
    if (ptr1 >= ptr2) {  // Line 16 - VIOLATION
        // Undefined behavior
    }
}

int main(void) {
    compare_tls();
    return 0;
}
