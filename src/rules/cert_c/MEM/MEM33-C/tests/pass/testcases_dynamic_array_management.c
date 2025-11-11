/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Pass Case: dynamic_array_management.c
 *
 * This case demonstrates compliant code that properly manages an array
 * of pointers to structures with flexible array members, showing correct
 * allocation, initialization, and cleanup patterns.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

/* COMPLIANT: Function to create a flexible array structure */
struct flex_array_struct *create_flex_struct(size_t size) {
    struct flex_array_struct *new_struct;

    if (size == 0) return NULL;

    new_struct = malloc(sizeof(struct flex_array_struct) + sizeof(int) * size);
    if (new_struct == NULL) return NULL;

    new_struct->num = size;
    for (size_t i = 0; i < size; i++) {
        new_struct->data[i] = (int)(i + 1);
    }

    return new_struct;
}

/* COMPLIANT: Function to safely free a flexible array structure */
void free_flex_struct(struct flex_array_struct *flex) {
    if (flex != NULL) {
        free(flex);
    }
}

int main(void) {
    const size_t array_count = 3;
    struct flex_array_struct *flex_array[array_count];
    size_t sizes[] = {2, 4, 3};

    /* COMPLIANT: Create array of pointers to flexible structures */
    for (size_t i = 0; i < array_count; i++) {
        flex_array[i] = create_flex_struct(sizes[i]);
        if (flex_array[i] == NULL) {
            /* Cleanup already allocated structures on failure */
            for (size_t j = 0; j < i; j++) {
                free_flex_struct(flex_array[j]);
            }
            return 1;
        }
    }

    /* Use the structures */
    printf("Array of flexible structures:\n");
    for (size_t i = 0; i < array_count; i++) {
        printf("Structure %zu (size %zu): ", i, flex_array[i]->num);
        for (size_t j = 0; j < flex_array[i]->num; j++) {
            printf("%d ", flex_array[i]->data[j]);
        }
        printf("\n");
    }

    /* COMPLIANT: Modify structures through pointers */
    printf("\nAfter modification:\n");
    for (size_t i = 0; i < array_count; i++) {
        /* Multiply each element by its structure index + 1 */
        for (size_t j = 0; j < flex_array[i]->num; j++) {
            flex_array[i]->data[j] *= (int)(i + 1);
        }

        printf("Structure %zu: ", i);
        for (size_t j = 0; j < flex_array[i]->num; j++) {
            printf("%d ", flex_array[i]->data[j]);
        }
        printf("\n");
    }

    /* COMPLIANT: Proper cleanup of all structures */
    for (size_t i = 0; i < array_count; i++) {
        free_flex_struct(flex_array[i]);
        flex_array[i] = NULL;  /* Prevent accidental reuse */
    }

    printf("\nAll structures properly freed\n");
    return 0;
}