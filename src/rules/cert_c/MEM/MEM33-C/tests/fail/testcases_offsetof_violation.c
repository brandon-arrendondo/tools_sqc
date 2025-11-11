/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: offsetof_violation.c
 *
 * This case demonstrates a violation of MEM33-C by using offsetof()
 * macro incorrectly with flexible array members, and making wrong
 * assumptions about memory layout and structure size.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    struct flex_array_struct *flex_struct;
    size_t array_size = 4;

    /* Print offsetof information */
    printf("offsetof(struct flex_array_struct, num) = %zu\n",
           offsetof(struct flex_array_struct, num));

    printf("offsetof(struct flex_array_struct, data) = %zu\n",
           offsetof(struct flex_array_struct, data));

    printf("sizeof(struct flex_array_struct) = %zu\n",
           sizeof(struct flex_array_struct));

    /* Proper allocation */
    flex_struct = malloc(sizeof(struct flex_array_struct) + sizeof(int) * array_size);
    if (flex_struct == NULL) return 1;

    flex_struct->num = array_size;
    for (size_t i = 0; i < array_size; i++) {
        flex_struct->data[i] = (int)(i * 3);
    }

    /* VIOLATION: Using offsetof to manually calculate addresses */
    char *base_addr = (char *)flex_struct;
    size_t data_offset = offsetof(struct flex_array_struct, data);

    /* VIOLATION: Manual pointer arithmetic based on offsetof */
    int *data_ptr = (int *)(base_addr + data_offset);

    printf("Manual address calculation:\n");
    for (size_t i = 0; i < array_size; i++) {
        printf("data[%zu] = %d\n", i, data_ptr[i]);
    }

    /* VIOLATION: Wrong size calculation using offsetof */
    size_t wrong_total_size = offsetof(struct flex_array_struct, data) + sizeof(int);

    struct flex_array_struct *wrong_struct = malloc(wrong_total_size);
    if (wrong_struct == NULL) {
        free(flex_struct);
        return 1;
    }

    wrong_struct->num = 3;  /* Claims 3 elements but only space for 1 */

    /* VIOLATION: Buffer overflow due to wrong size calculation */
    wrong_struct->data[0] = 10;
    wrong_struct->data[1] = 20;  /* Buffer overflow */
    wrong_struct->data[2] = 30;  /* Buffer overflow */

    printf("Wrong allocation results:\n");
    for (size_t i = 0; i < wrong_struct->num; i++) {
        printf("wrong_data[%zu] = %d\n", i, wrong_struct->data[i]);
    }

    free(flex_struct);
    free(wrong_struct);
    return 0;
}