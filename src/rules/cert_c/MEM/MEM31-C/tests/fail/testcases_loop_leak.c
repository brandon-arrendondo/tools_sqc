/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Memory allocated in loop is never freed
 */

#include <stdlib.h>

void allocate_in_loop() {
    for (int i = 0; i < 10; i++) {
        char *buffer = malloc(100);
        if (buffer != NULL) {
            // Use the buffer
            buffer[0] = 'A' + i;
        }
        // Memory is never freed - 10 memory leaks
    }
}