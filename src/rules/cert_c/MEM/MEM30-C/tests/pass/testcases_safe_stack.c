/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: PASS
 * Reason: Uses stack allocation instead of heap, automatic cleanup at scope end
 */

#include <stdio.h>

void process_stack_data() {
    int data[10];  // Stack allocated

    // Initialize and use
    for (int i = 0; i < 10; i++) {
        data[i] = i * i;
    }

    for (int i = 0; i < 10; i++) {
        printf("data[%d] = %d\n", i, data[i]);
    }

    // No explicit free needed, automatic cleanup
}

int main() {
    process_stack_data();
    // Stack memory automatically cleaned up
    return 0;
}