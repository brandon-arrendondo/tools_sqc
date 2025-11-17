/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM31-C violation
 */

/*
 * Rule: MEM31-C - Free dynamically allocated memory when no longer needed
 * Status: FAIL
 * Reason: Multiple allocations to same pointer without freeing previous
 */

#include <stdlib.h>

void double_allocation() {
    char *ptr = malloc(100);
    if (ptr == NULL) {
        return;
    }

    ptr[0] = 'A';

    // Allocate again without freeing previous
    ptr = malloc(200);  // Previous 100 bytes leaked
    if (ptr == NULL) {
        return;
    }

    ptr[0] = 'B';

    // Allocate third time
    ptr = malloc(300);  // Previous 200 bytes leaked
    if (ptr != NULL) {
        ptr[0] = 'C';
        free(ptr);  // Only frees the last allocation
    }
}