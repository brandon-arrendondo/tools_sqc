/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: array_initialization.c
 *
 * This case demonstrates a violation of MEM33-C by attempting to initialize
 * a structure with flexible array member using compound literals or
 * initializers, which is invalid for flexible array members.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    /* VIOLATION: Attempting to initialize flexible array structure with compound literal */
    struct flex_array_struct *flex_ptr = &(struct flex_array_struct){
        .num = 3,
        .data = {10, 20, 30}  /* Invalid - cannot initialize flexible array */
    };

    printf("Number of elements: %zu\n", flex_ptr->num);

    /* Accessing data results in undefined behavior */
    for (size_t i = 0; i < flex_ptr->num; i++) {
        printf("data[%zu] = %d\n", i, flex_ptr->data[i]);
    }

    return 0;
}