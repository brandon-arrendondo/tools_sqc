/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Recursion creates multiple allocations that are never freed
 */

#include <stdlib.h>

void recursive_allocate(int depth) {
    if (depth <= 0) {
        return;
    }

    char *buffer = malloc(100);
    if (buffer == NULL) {
        return;
    }

    buffer[0] = 'A' + depth;
    printf("Allocated at depth %d\n", depth);

    recursive_allocate(depth - 1);

    // Each recursive call allocates but never frees - MEMORY LEAK
}