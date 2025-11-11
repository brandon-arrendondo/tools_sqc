/*
 * Rule: MEM31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

/*
 * MEM31-C PASS Case: Single allocation and deallocation
 *
 * This test case demonstrates the fundamental principle of MEM31-C:
 * every dynamically allocated memory block must be freed exactly once.
 * This is the foundation of secure memory management in C.
 *
 * Compliant practices demonstrated:
 * - Single malloc() call
 * - Check allocation success before use
 * - Use allocated memory safely
 * - Single free() call
 * - Defensive programming with pointer nullification
 *
 * Security benefits:
 * - Prevents double-free vulnerabilities
 * - Avoids memory leaks
 * - Defensive against accidental reuse
 * - Clear ownership semantics
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    // Single allocation - establish clear ownership
    int *ptr = malloc(10 * sizeof(int));

    if (ptr != NULL) {
        // Use the allocated memory safely
        for (int i = 0; i < 10; i++) {
            ptr[i] = i * i;
        }

        // Print some values to demonstrate usage
        printf("ptr[5] = %d\n", ptr[5]);

        // CRITICAL: Free exactly once
        free(ptr);

        // DEFENSIVE PROGRAMMING: Nullify pointer after freeing
        // This prevents accidental reuse and makes double-free attempts safe
        ptr = NULL;

        printf("Memory freed successfully\n");
    } else {
        printf("Memory allocation failed\n");
        // No free() needed for NULL pointer - free(NULL) is safe but unnecessary
    }

    return 0;
}