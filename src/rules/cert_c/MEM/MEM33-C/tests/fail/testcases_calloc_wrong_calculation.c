/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: calloc_wrong_calculation.c
 *
 * This case demonstrates a violation of MEM33-C by using calloc() with
 * incorrect parameters for allocating a structure with flexible array
 * member. The size calculation must be done correctly even with calloc().
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    struct flex_array_struct *flex_struct;
    size_t array_size = 8;

    /* VIOLATION: Incorrect calloc usage - only allocating space for fixed members */
    flex_struct = calloc(1, sizeof(struct flex_array_struct));
    if (flex_struct == NULL) return 1;

    flex_struct->num = array_size;

    /* VIOLATION: Writing to flexible array without proper space allocation */
    printf("Writing to unallocated flexible array space:\n");
    for (size_t i = 0; i < array_size; i++) {
        flex_struct->data[i] = (int)(i * 3);  /* Buffer overflow */
        printf("data[%zu] = %d\n", i, flex_struct->data[i]);
    }

    /* Another violation: using calloc incorrectly for flexible array */
    free(flex_struct);

    /* VIOLATION: Wrong calloc parameters */
    flex_struct = calloc(sizeof(struct flex_array_struct), array_size);
    if (flex_struct == NULL) return 1;

    /* This allocates way too much memory and doesn't properly align the flexible array */
    flex_struct->num = array_size;
    flex_struct->data[0] = 999;  /* May work but structure is incorrect */

    printf("Incorrectly allocated data[0] = %d\n", flex_struct->data[0]);

    free(flex_struct);
    return 0;
}