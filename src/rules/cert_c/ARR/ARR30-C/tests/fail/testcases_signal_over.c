/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Array overflow in signal handler context
 */

#include <stdio.h>
#include <signal.h>
#include <string.h>

volatile sig_atomic_t signal_count[5] = {0};

void signal_handler(int signum) {
    // Overflow in signal handler - very dangerous
    signal_count[5]++;   // Line 15 - VIOLATION (index 5 >= size 5)
    signal_count[10]++;  // Line 16 - VIOLATION
}

int main(void) {
    signal(SIGINT, signal_handler);

    // Also violate in main context
    signal_count[6] = 100;  // Line 22 - VIOLATION

    printf("Signal counts initialized\n");
    return 0;
}
