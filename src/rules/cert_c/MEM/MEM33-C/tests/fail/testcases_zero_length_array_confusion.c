/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: zero_length_array_confusion.c
 *
 * This case demonstrates a violation of MEM33-C by confusing zero-length
 * arrays (a GCC extension) with flexible array members, and using
 * inappropriate allocation strategies that don't follow MEM33-C.
 */

#include <stdio.h>
#include <stdlib.h>

/* This looks like a flexible array but uses GCC zero-length extension */
struct zero_length_struct {
    size_t num;
    int data[0];  /* VIOLATION: Zero-length array, not standard flexible array */
};

/* Standard flexible array for comparison */
struct flex_array_struct {
    size_t num;
    int data[];  /* Correct flexible array member */
};

int main(void) {
    /* VIOLATION: Treating zero-length array struct like regular struct */
    struct zero_length_struct zero_struct;
    zero_struct.num = 3;

    /* VIOLATION: Accessing zero-length array without proper allocation */
    zero_struct.data[0] = 10;  /* Undefined behavior */
    zero_struct.data[1] = 20;  /* Undefined behavior */
    zero_struct.data[2] = 30;  /* Undefined behavior */

    printf("Zero-length array struct:\n");
    printf("num: %zu\n", zero_struct.num);
    for (size_t i = 0; i < zero_struct.num; i++) {
        printf("data[%zu] = %d\n", i, zero_struct.data[i]);
    }

    /* VIOLATION: Incorrect allocation assuming zero-length array behavior */
    struct flex_array_struct *flex_ptr;

    /* Wrong assumption: thinking data[0] means fixed size 0 */
    flex_ptr = malloc(sizeof(struct flex_array_struct));  /* Insufficient space */
    if (flex_ptr == NULL) return 1;

    flex_ptr->num = 2;

    /* VIOLATION: Writing to flexible array without proper space */
    flex_ptr->data[0] = 100;  /* Buffer overflow */
    flex_ptr->data[1] = 200;  /* Buffer overflow */

    printf("\nFlexible array with wrong allocation:\n");
    printf("data[0] = %d, data[1] = %d\n", flex_ptr->data[0], flex_ptr->data[1]);

    free(flex_ptr);
    return 0;
}