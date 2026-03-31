/*
 * Rule: EXP34-C
 * Source: testcases (Phase 4 regression)
 * Status: PASS - Global pointer may be NULL, but is always checked before use.
 */

#include <stdio.h>

int *global_ptr = NULL;

void use_global_safely(void) {
    if (global_ptr != NULL) {
        *global_ptr = 42;
        printf("Value: %d\n", *global_ptr);
    }
}

int main() {
    use_global_safely();
    return 0;
}
