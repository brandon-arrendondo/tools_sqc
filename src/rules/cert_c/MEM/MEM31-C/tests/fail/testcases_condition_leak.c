/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Memory freed only in one branch of conditional
 */

#include <stdlib.h>

void conditional_free(int flag) {
    char *buffer = malloc(256);
    if (buffer == NULL) {
        return;
    }

    if (flag > 0) {
        buffer[0] = 'P';
        free(buffer);  // Only freed in positive case
    } else {
        buffer[0] = 'N';
        // buffer not freed in negative case - MEMORY LEAK
    }
}