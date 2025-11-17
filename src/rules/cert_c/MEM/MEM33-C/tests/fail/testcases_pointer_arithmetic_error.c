/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: pointer_arithmetic_error.c
 *
 * This case demonstrates a violation of MEM33-C by using incorrect pointer
 * arithmetic when working with structures containing flexible array members.
 * The size calculations must account for the flexible array.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    struct flex_array_struct *flex_array[3];
    size_t array_size = 5;

    /* Allocate multiple structures */
    for (int i = 0; i < 3; i++) {
        flex_array[i] = malloc(sizeof(struct flex_array_struct) + sizeof(int) * array_size);
        if (flex_array[i] == NULL) return 1;

        flex_array[i]->num = array_size;
        for (size_t j = 0; j < array_size; j++) {
            flex_array[i]->data[j] = (int)(i * 10 + j);
        }
    }

    /* VIOLATION: Incorrect pointer arithmetic - treating as fixed-size structs */
    struct flex_array_struct *ptr = flex_array[0];
    for (int i = 0; i < 3; i++) {
        printf("Struct %d num: %zu\n", i, ptr->num);

        /* VIOLATION: Moving pointer by sizeof(struct) doesn't account for flexible array */
        ptr = (struct flex_array_struct *)((char *)ptr + sizeof(struct flex_array_struct));
        /* This points to invalid memory locations */
    }

    /* Cleanup */
    for (int i = 0; i < 3; i++) {
        free(flex_array[i]);
    }

    return 0;
}