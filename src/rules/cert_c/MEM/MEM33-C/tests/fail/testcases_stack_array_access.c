/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: stack_array_access.c
 *
 * This case demonstrates a violation of MEM33-C by attempting to access
 * the flexible array member of a structure allocated on the stack.
 * The flexible array has no allocated space in automatic storage.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

void process_flex_array(void) {
    /* VIOLATION: Automatic storage for flexible array structure */
    struct flex_array_struct local_flex;

    local_flex.num = 4;

    /* VIOLATION: Writing to flexible array without proper allocation */
    local_flex.data[0] = 100;  /* Undefined behavior */
    local_flex.data[1] = 200;  /* Undefined behavior */
    local_flex.data[2] = 300;  /* Undefined behavior */
    local_flex.data[3] = 400;  /* Undefined behavior */

    /* Reading also causes undefined behavior */
    printf("Stack flex array contents:\n");
    for (size_t i = 0; i < local_flex.num; i++) {
        printf("data[%zu] = %d\n", i, local_flex.data[i]);
    }
}

int main(void) {
    process_flex_array();
    return 0;
}