/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: incorrect_size_calculation.c
 *
 * This case demonstrates a violation of MEM33-C by using incorrect size
 * calculation when allocating memory for a structure with flexible array
 * member. Using sizeof() on the struct doesn't account for the flexible array.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    struct flex_array_struct *flex_struct;
    size_t array_size = 10;

    /* VIOLATION: Incorrect size calculation - only allocates space for fixed members */
    flex_struct = malloc(sizeof(struct flex_array_struct));
    if (flex_struct == NULL) return 1;

    flex_struct->num = array_size;

    /* VIOLATION: Writing to data array without proper space allocation */
    for (size_t i = 0; i < array_size; i++) {
        flex_struct->data[i] = (int)i;  /* Undefined behavior - buffer overflow */
    }

    /* Reading the data also causes undefined behavior */
    printf("First element: %d\n", flex_struct->data[0]);

    free(flex_struct);
    return 0;
}