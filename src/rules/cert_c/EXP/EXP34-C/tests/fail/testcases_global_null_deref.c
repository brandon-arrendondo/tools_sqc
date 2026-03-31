/*
 * Rule: EXP34-C
 * Source: testcases (Phase 4 regression)
 * Status: FAIL - File-scope global pointer initialized to NULL, then dereferenced
 *         in a function without a null check.
 */

#include <stdio.h>

int *global_ptr = NULL;

void use_global(void) {
    /* Dereference of file-scope global known to be NULL */
    *global_ptr = 42;
    printf("Value: %d\n", *global_ptr);
}

int main() {
    use_global();
    return 0;
}
