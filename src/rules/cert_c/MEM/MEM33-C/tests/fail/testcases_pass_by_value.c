/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: pass_by_value.c
 *
 * This case demonstrates a violation of MEM33-C by attempting to pass a
 * structure containing a flexible array member by value to a function.
 * This only copies the fixed members and loses the flexible array data.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

/* VIOLATION: Function accepts flexible array struct by value */
void process_struct(struct flex_array_struct flex_struct) {
    /* flex_struct.data is not properly copied here */
    printf("Processing struct with %zu elements\n", flex_struct.num);

    /* Accessing flex_struct.data results in undefined behavior */
    if (flex_struct.num > 0) {
        printf("First element: %d\n", flex_struct.data[0]);
    }
}

int main(void) {
    struct flex_array_struct *flex_struct;
    size_t array_size = 5;

    flex_struct = malloc(sizeof(struct flex_array_struct) + sizeof(int) * array_size);
    if (flex_struct == NULL) return 1;

    flex_struct->num = array_size;
    for (size_t i = 0; i < array_size; i++) {
        flex_struct->data[i] = (int)i * 10;
    }

    /* VIOLATION: Passing by value only copies fixed members */
    process_struct(*flex_struct);

    free(flex_struct);
    return 0;
}