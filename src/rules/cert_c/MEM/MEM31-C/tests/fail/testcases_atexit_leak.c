/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Memory allocated but program exits before cleanup
 */

#include <stdlib.h>

char *global_resource = NULL;

void cleanup_handler() {
    printf("Cleanup handler called\n");
    // Should free global_resource here but doesn't
}

void allocate_and_exit() {
    atexit(cleanup_handler);

    global_resource = malloc(1024);
    if (global_resource == NULL) {
        return;
    }

    global_resource[0] = 'E';

    // Simulate early program termination
    exit(0);  // global_resource is never freed - MEMORY LEAK
}