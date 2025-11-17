/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: const_flex_struct.c
 *
 * This case demonstrates a violation of MEM33-C by attempting to use
 * const-qualified structures with flexible array members in automatic
 * storage, which is invalid.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    /* VIOLATION: const flexible array structure in automatic storage */
    const struct flex_array_struct const_flex = {
        .num = 3
        /* Cannot initialize flexible array member */
    };

    printf("Const struct num: %zu\n", const_flex.num);

    /* VIOLATION: Accessing flexible array member of const struct in automatic storage */
    printf("Attempting to read data[0]: %d\n", const_flex.data[0]);  /* Undefined behavior */

    /* Another violation: attempting to modify through cast */
    struct flex_array_struct *non_const = (struct flex_array_struct *)&const_flex;
    non_const->data[0] = 42;  /* Undefined behavior */

    printf("Modified data[0]: %d\n", non_const->data[0]);

    return 0;
}