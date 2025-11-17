/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Frees memory through one union member, accesses through another
 */

#include <stdlib.h>
#include <stdio.h>

typedef union {
    int *int_ptr;
    char *char_ptr;
    void *void_ptr;
} ptr_union_t;

int main() {
    ptr_union_t u;
    u.int_ptr = malloc(sizeof(int));
    if (u.int_ptr == NULL) {
        return -1;
    }

    *(u.int_ptr) = 888;

    // Free through void_ptr member
    free(u.void_ptr);

    // BUG: Access through int_ptr member (same memory)
    printf("Value: %d\n", *(u.int_ptr));

    return 0;
}