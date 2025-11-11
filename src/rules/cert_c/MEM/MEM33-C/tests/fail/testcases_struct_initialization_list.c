/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: struct_initialization_list.c
 *
 * This case demonstrates a violation of MEM33-C by attempting to use
 * struct initialization lists with flexible array members in automatic
 * storage, which is invalid syntax and behavior.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    /* VIOLATION: Attempting initialization with flexible array in automatic storage */
    struct flex_array_struct local_flex = {
        .num = 3,
        /* Cannot initialize flexible array member */
    };

    printf("Initialized struct num: %zu\n", local_flex.num);

    /* VIOLATION: Accessing uninitialized flexible array */
    local_flex.data[0] = 5;   /* Undefined behavior */
    local_flex.data[1] = 10;  /* Undefined behavior */
    local_flex.data[2] = 15;  /* Undefined behavior */

    printf("Local flex data: ");
    for (size_t i = 0; i < local_flex.num; i++) {
        printf("%d ", local_flex.data[i]);
    }
    printf("\n");

    /* Another violation: compound literal with flexible array */
    struct flex_array_struct *ptr = &(struct flex_array_struct){
        .num = 2,
        /* Flexible array cannot be initialized this way */
    };

    /* VIOLATION: Accessing flexible array in compound literal */
    ptr->data[0] = 100;  /* Undefined behavior */
    ptr->data[1] = 200;  /* Undefined behavior */

    printf("Compound literal data: %d, %d\n", ptr->data[0], ptr->data[1]);

    return 0;
}