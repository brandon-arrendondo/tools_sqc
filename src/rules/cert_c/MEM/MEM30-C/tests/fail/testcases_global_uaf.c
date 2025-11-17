/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Global pointer freed in one function, accessed in another
 */

#include <stdlib.h>
#include <stdio.h>

int *global_ptr;

void allocate_global() {
    global_ptr = malloc(sizeof(int));
    if (global_ptr != NULL) {
        *global_ptr = 123;
    }
}

void free_global() {
    free(global_ptr);
    // BUG: Should set to NULL but doesn't
}

void use_global() {
    // BUG: Access potentially freed global
    if (global_ptr != NULL) {
        printf("Global value: %d\n", *global_ptr);
    }
}

int main() {
    allocate_global();
    free_global();
    use_global();  // Uses freed memory

    return 0;
}