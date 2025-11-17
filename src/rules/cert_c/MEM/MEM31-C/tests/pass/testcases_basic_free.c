/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: PASS
 * Reason: Memory is allocated and properly freed before function returns
 */

#include <stdlib.h>
#include <string.h>

void process_data(size_t size) {
    char *buffer = malloc(size);
    if (buffer == NULL) {
        return;
    }

    // Use the buffer for some processing
    memset(buffer, 0, size);

    // Properly free the allocated memory
    free(buffer);
}