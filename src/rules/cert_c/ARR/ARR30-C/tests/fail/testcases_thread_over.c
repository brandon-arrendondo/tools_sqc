/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Thread-local array accessed beyond declared bounds
 */

#include <stdio.h>
#include <threads.h>

_Thread_local int thread_data[8] = {1, 2, 3, 4, 5, 6, 7, 8};

int thread_function(void *arg) {
    // Access beyond thread-local array bounds
    thread_data[10] = 999;  // Line 14 - VIOLATION
    return thread_data[12];  // Line 15 - VIOLATION
}

int main(void) {
    thread_data[15] = 100;  // Line 19 - VIOLATION
    printf("Thread-local data: %d\n", thread_data[15]);  // Line 20 - VIOLATION (same line, reading)
    return 0;
}
