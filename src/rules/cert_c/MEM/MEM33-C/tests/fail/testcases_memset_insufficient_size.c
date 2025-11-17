/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Fail Case: memset_insufficient_size.c
 *
 * This case demonstrates a violation of MEM33-C by using memset() with
 * incorrect size calculation for structures with flexible array members,
 * failing to initialize the flexible array portion.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    struct flex_array_struct *flex_struct;
    size_t array_size = 6;

    /* Proper allocation */
    flex_struct = malloc(sizeof(struct flex_array_struct) + sizeof(int) * array_size);
    if (flex_struct == NULL) return 1;

    /* VIOLATION: memset with incorrect size - only clears fixed members */
    memset(flex_struct, 0, sizeof(struct flex_array_struct));

    flex_struct->num = array_size;

    /* The flexible array portion contains garbage values */
    printf("After memset with wrong size:\n");
    printf("num: %zu\n", flex_struct->num);

    /* These values are uninitialized garbage */
    for (size_t i = 0; i < array_size; i++) {
        printf("data[%zu] = %d (garbage)\n", i, flex_struct->data[i]);
    }

    /* Another violation: using memset after data initialization */
    for (size_t i = 0; i < array_size; i++) {
        flex_struct->data[i] = (int)(i + 10);
    }

    printf("\nAfter proper initialization:\n");
    for (size_t i = 0; i < array_size; i++) {
        printf("data[%zu] = %d\n", i, flex_struct->data[i]);
    }

    /* VIOLATION: memset again with wrong size, losing data */
    memset(flex_struct, 0xFF, sizeof(struct flex_array_struct));

    printf("\nAfter second memset with wrong size:\n");
    printf("num: %zu (corrupted)\n", flex_struct->num);

    free(flex_struct);
    return 0;
}