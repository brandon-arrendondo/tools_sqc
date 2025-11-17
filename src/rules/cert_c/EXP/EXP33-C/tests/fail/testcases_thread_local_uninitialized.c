/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: thread_local_uninitialized.c
 */

#include <stdio.h>

/* NON-COMPLIANT: Thread-local storage uninitialized */
_Thread_local int thread_data;  /* Uninitialized thread-local */

void unsafe_thread_function(void) {
    thread_data += 10;  /* Uses uninitialized thread-local data */
    printf("Thread data: %d\n", thread_data);
}

int main(void) {
    unsafe_thread_function();
    return 0;
}