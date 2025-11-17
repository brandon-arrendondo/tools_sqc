/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Local pointer variable goes out of scope without freeing memory
 */

#include <stdlib.h>

void scope_problem() {
    {
        char *local_ptr = malloc(128);
        if (local_ptr != NULL) {
            local_ptr[0] = 'L';
        }
        // local_ptr goes out of scope here without being freed
    } // MEMORY LEAK - pointer is lost but memory remains allocated

    printf("Continuing execution\n");
}