/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Global pointer reassigned without freeing previous allocation
 */

#include <stdlib.h>

char *global_buffer = NULL;

void init_global_buffer() {
    global_buffer = malloc(1024);
    if (global_buffer != NULL) {
        global_buffer[0] = 'G';
    }
}

void reinit_global_buffer() {
    // Reassigning without freeing previous allocation
    global_buffer = malloc(2048);  // Previous allocation leaked
    if (global_buffer != NULL) {
        global_buffer[0] = 'R';
    }
}

void cleanup() {
    free(global_buffer);  // Only frees the last allocation
}