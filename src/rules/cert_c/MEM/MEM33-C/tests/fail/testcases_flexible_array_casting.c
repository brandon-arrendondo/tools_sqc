/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: flexible_array_casting.c
 *
 * This case demonstrates a violation of MEM33-C by improper casting
 * and pointer manipulation with structures containing flexible array
 * members, leading to incorrect memory access patterns.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

struct regular_struct {
    size_t num;
    int fixed_data[5];  /* Fixed-size array */
};

int main(void) {
    struct flex_array_struct *flex_struct;
    struct regular_struct regular;
    size_t array_size = 5;

    /* Proper allocation for flexible array structure */
    flex_struct = malloc(sizeof(struct flex_array_struct) + sizeof(int) * array_size);
    if (flex_struct == NULL) return 1;

    flex_struct->num = array_size;
    for (size_t i = 0; i < array_size; i++) {
        flex_struct->data[i] = (int)(i * 2);
    }

    /* Initialize regular struct */
    regular.num = 5;
    for (int i = 0; i < 5; i++) {
        regular.fixed_data[i] = i + 10;
    }

    /* VIOLATION: Casting regular struct to flexible array struct */
    struct flex_array_struct *bad_cast = (struct flex_array_struct *)&regular;

    printf("Bad cast access:\n");
    printf("num: %zu\n", bad_cast->num);

    /* VIOLATION: Accessing flexible array through improper cast */
    for (size_t i = 0; i < bad_cast->num && i < 3; i++) {
        printf("data[%zu] = %d\n", i, bad_cast->data[i]);  /* May access wrong memory */
    }

    /* VIOLATION: Casting flexible array struct to regular struct */
    struct regular_struct *another_bad_cast = (struct regular_struct *)flex_struct;

    printf("\nAnother bad cast:\n");
    printf("num: %zu\n", another_bad_cast->num);

    /* VIOLATION: Treating flexible array as fixed array */
    for (int i = 0; i < 5; i++) {
        printf("fixed_data[%d] = %d\n", i, another_bad_cast->fixed_data[i]);
    }

    /* VIOLATION: Pointer arithmetic assuming fixed size */
    struct flex_array_struct *wrong_ptr = flex_struct + 1;  /* Wrong calculation */
    printf("\nWrong pointer arithmetic result: %p\n", (void *)wrong_ptr);

    free(flex_struct);
    return 0;
}