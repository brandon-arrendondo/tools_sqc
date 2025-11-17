/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Only some of the allocated memory blocks are freed
 */

#include <stdlib.h>

void allocate_multiple() {
    char *buffer1 = malloc(100);
    char *buffer2 = malloc(200);
    char *buffer3 = malloc(300);

    if (buffer1 == NULL || buffer2 == NULL || buffer3 == NULL) {
        return;  // Should free any successful allocations
    }

    // Use all buffers
    buffer1[0] = 'A';
    buffer2[0] = 'B';
    buffer3[0] = 'C';

    // Only free some buffers
    free(buffer1);
    free(buffer2);
    // buffer3 is never freed - MEMORY LEAK
}