/*
 * Rule: EXP34-C
 * Source: testcases (Phase 4 regression)
 * Status: FAIL - Global pointer assigned NULL inside a function, then
 *         dereferenced later without a null check.
 */

#include <stdio.h>

int *shared_data;

void initialize_bad(void) {
    shared_data = NULL;
}

void process(void) {
    /* Dereference after function assigns NULL */
    printf("Value: %d\n", *shared_data);
}

int main() {
    initialize_bad();
    process();
    return 0;
}
