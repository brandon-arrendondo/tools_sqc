/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Macro expansion creates multiple returns without proper cleanup
 */

#include <stdlib.h>

#define CHECK_AND_RETURN(condition) \
    if (condition) return

void macro_function(int value) {
    char *buffer = malloc(200);
    if (buffer == NULL) {
        return;
    }

    buffer[0] = 'M';

    CHECK_AND_RETURN(value < 0);     // Early return without free - MEMORY LEAK
    CHECK_AND_RETURN(value > 100);   // Another early return - MEMORY LEAK

    buffer[1] = 'A';

    free(buffer);  // Only reached if 0 <= value <= 100
}