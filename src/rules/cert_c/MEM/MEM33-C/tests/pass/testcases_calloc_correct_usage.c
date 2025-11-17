/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Pass Case: calloc_correct_usage.c
 *
 * This case demonstrates compliant code that properly uses calloc()
 * to allocate zeroed memory for a structure containing a flexible
 * array member with correct size calculations.
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
    size_t total_size;

    /* COMPLIANT: Proper calloc usage with correct total size */
    total_size = sizeof(struct flex_array_struct) + sizeof(int) * array_size;
    flex_struct = calloc(1, total_size);

    if (flex_struct == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    /* Set the size - calloc zeroed all memory */
    flex_struct->num = array_size;

    printf("After calloc (all zeros):\n");
    printf("Number of elements: %zu\n", flex_struct->num);
    printf("Initial values: ");
    for (size_t i = 0; i < flex_struct->num; i++) {
        printf("%d ", flex_struct->data[i]);
    }
    printf("\n");

    /* Initialize with meaningful data */
    for (size_t i = 0; i < array_size; i++) {
        flex_struct->data[i] = (int)(i * 5 + 1);
    }

    printf("\nAfter initialization:\n");
    printf("Values: ");
    for (size_t i = 0; i < flex_struct->num; i++) {
        printf("%d ", flex_struct->data[i]);
    }
    printf("\n");

    /* Alternative: using calloc for array of bytes then casting */
    free(flex_struct);

    /* COMPLIANT: Alternative approach with byte allocation */
    char *raw_memory = calloc(total_size, sizeof(char));
    if (raw_memory == NULL) {
        fprintf(stderr, "Raw memory allocation failed\n");
        return 1;
    }

    flex_struct = (struct flex_array_struct *)raw_memory;
    flex_struct->num = array_size;

    /* Fill with pattern */
    for (size_t i = 0; i < array_size; i++) {
        flex_struct->data[i] = (int)(i + 10);
    }

    printf("\nAlternative calloc approach:\n");
    printf("Values: ");
    for (size_t i = 0; i < flex_struct->num; i++) {
        printf("%d ", flex_struct->data[i]);
    }
    printf("\n");

    /* COMPLIANT: Proper cleanup */
    free(raw_memory);
    return 0;
}