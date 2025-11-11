/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Memory is allocated but never freed, causing a memory leak
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

    // Memory is never freed - MEMORY LEAK
}