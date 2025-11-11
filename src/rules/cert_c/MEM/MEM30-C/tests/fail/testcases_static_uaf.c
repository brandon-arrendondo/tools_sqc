/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Static pointer retains freed memory address across function calls
 */

#include <stdlib.h>
#include <stdio.h>

int *get_static_ptr() {
    static int *static_ptr = NULL;

    if (static_ptr == NULL) {
        static_ptr = malloc(sizeof(int));
        *static_ptr = 123;
    }

    free(static_ptr);
    // BUG: Don't set to NULL, so it retains freed address

    return static_ptr;
}

int main() {
    int *ptr1 = get_static_ptr();
    int *ptr2 = get_static_ptr();

    // BUG: Both pointers point to freed memory
    printf("Value 1: %d\n", *ptr1);
    printf("Value 2: %d\n", *ptr2);

    return 0;
}