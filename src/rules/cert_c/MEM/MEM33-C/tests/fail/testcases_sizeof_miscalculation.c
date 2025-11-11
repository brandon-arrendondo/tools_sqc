/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: sizeof_miscalculation.c
 *
 * This case demonstrates a violation of MEM33-C by using sizeof() operator
 * incorrectly with flexible array structures, leading to insufficient
 * memory allocation and buffer overflows.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    struct flex_array_struct *flex_struct;
    size_t array_size = 6;

    /* Print the sizeof for demonstration */
    printf("sizeof(struct flex_array_struct) = %zu\n",
           sizeof(struct flex_array_struct));

    /* VIOLATION: Using sizeof without accounting for flexible array */
    size_t total_size = sizeof(*flex_struct);  /* Wrong calculation */
    flex_struct = malloc(total_size);

    if (flex_struct == NULL) return 1;

    flex_struct->num = array_size;

    /* VIOLATION: Writing beyond allocated memory */
    printf("Attempting to write %zu elements:\n", array_size);
    for (size_t i = 0; i < array_size; i++) {
        flex_struct->data[i] = (int)(i * 5);  /* Buffer overflow */
        printf("Wrote data[%zu] = %d\n", i, flex_struct->data[i]);
    }

    free(flex_struct);
    return 0;
}