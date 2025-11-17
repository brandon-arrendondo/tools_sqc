/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: realloc_incorrect_size.c
 *
 * This case demonstrates a violation of MEM33-C by using realloc() with
 * incorrect size calculation for a structure containing a flexible array
 * member. The new size must include space for the flexible array.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    struct flex_array_struct *flex_struct;
    size_t initial_size = 5;
    size_t new_size = 10;

    /* Initial proper allocation */
    flex_struct = malloc(sizeof(struct flex_array_struct) + sizeof(int) * initial_size);
    if (flex_struct == NULL) return 1;

    flex_struct->num = initial_size;
    for (size_t i = 0; i < initial_size; i++) {
        flex_struct->data[i] = (int)(i + 1);
    }

    /* VIOLATION: Incorrect realloc size - only accounts for fixed members */
    flex_struct = realloc(flex_struct, sizeof(struct flex_array_struct));
    if (flex_struct == NULL) return 1;

    /* VIOLATION: Accessing flexible array with insufficient memory */
    flex_struct->num = new_size;
    for (size_t i = initial_size; i < new_size; i++) {
        flex_struct->data[i] = (int)(i + 1);  /* Buffer overflow */
    }

    printf("Accessing potentially invalid memory:\n");
    for (size_t i = 0; i < new_size; i++) {
        printf("data[%zu] = %d\n", i, flex_struct->data[i]);
    }

    free(flex_struct);
    return 0;
}