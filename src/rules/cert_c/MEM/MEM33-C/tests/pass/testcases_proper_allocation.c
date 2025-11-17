/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Pass Case: proper_allocation.c
 *
 * This case demonstrates compliant code that properly allocates memory
 * for a structure containing a flexible array member using dynamic allocation
 * with correct size calculation.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    struct flex_array_struct *flex_struct;
    size_t array_size = 5;

    /* COMPLIANT: Proper dynamic allocation with correct size calculation */
    flex_struct = malloc(sizeof(struct flex_array_struct) + sizeof(int) * array_size);
    if (flex_struct == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    /* Initialize the structure */
    flex_struct->num = array_size;
    for (size_t i = 0; i < array_size; i++) {
        flex_struct->data[i] = (int)(i * 10);
    }

    /* Use the structure */
    printf("Properly allocated flexible array structure:\n");
    printf("Number of elements: %zu\n", flex_struct->num);
    printf("Elements: ");
    for (size_t i = 0; i < flex_struct->num; i++) {
        printf("%d ", flex_struct->data[i]);
    }
    printf("\n");

    /* COMPLIANT: Proper cleanup */
    free(flex_struct);
    return 0;
}