/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: PASS
 * Reason: All memory allocated in loop is properly freed
 */

#include <stdlib.h>

void process_multiple_buffers() {
    char *buffers[5];
    int allocated_count = 0;

    // Allocate multiple buffers
    for (int i = 0; i < 5; i++) {
        buffers[i] = malloc(100);
        if (buffers[i] == NULL) {
            // If allocation fails, free previously allocated buffers
            for (int j = 0; j < allocated_count; j++) {
                free(buffers[j]);
            }
            return;
        }
        allocated_count++;
    }

    // Use the buffers
    for (int i = 0; i < 5; i++) {
        sprintf(buffers[i], "Buffer %d content", i);
    }

    // Properly free all allocated buffers
    for (int i = 0; i < 5; i++) {
        free(buffers[i]);
    }
}