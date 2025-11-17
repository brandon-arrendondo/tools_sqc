/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Pass Case: memset_full_size.c
 *
 * This case demonstrates compliant code that properly uses memset()
 * with structures containing flexible array members by calculating
 * the correct total size including the flexible array portion.
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
    size_t array_size = 5;
    size_t total_size;

    /* COMPLIANT: Proper dynamic allocation */
    total_size = sizeof(struct flex_array_struct) + sizeof(int) * array_size;
    flex_struct = malloc(total_size);

    if (flex_struct == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    /* COMPLIANT: memset with correct total size */
    memset(flex_struct, 0, total_size);

    /* Set the number of elements */
    flex_struct->num = array_size;

    printf("After memset (all zeros):\n");
    printf("num: %zu\n", flex_struct->num);
    printf("data: ");
    for (size_t i = 0; i < flex_struct->num; i++) {
        printf("%d ", flex_struct->data[i]);
    }
    printf("\n");

    /* Initialize with data */
    for (size_t i = 0; i < array_size; i++) {
        flex_struct->data[i] = (int)(i * 3 + 7);
    }

    printf("\nAfter initialization:\n");
    printf("data: ");
    for (size_t i = 0; i < flex_struct->num; i++) {
        printf("%d ", flex_struct->data[i]);
    }
    printf("\n");

    /* COMPLIANT: Selective memset of just the flexible array portion */
    char *array_start = (char *)flex_struct->data;
    size_t array_byte_size = sizeof(int) * array_size;
    memset(array_start, 0xFF, array_byte_size);

    printf("\nAfter memset of array portion (0xFF pattern):\n");
    printf("num: %zu (unchanged)\n", flex_struct->num);
    printf("data (as unsigned): ");
    for (size_t i = 0; i < flex_struct->num; i++) {
        printf("%u ", (unsigned int)flex_struct->data[i]);
    }
    printf("\n");

    /* COMPLIANT: Reset to zeros using correct total size */
    memset(flex_struct, 0, total_size);
    flex_struct->num = array_size;  /* Restore size after zeroing */

    printf("\nAfter full reset:\n");
    printf("num: %zu\n", flex_struct->num);
    printf("data: ");
    for (size_t i = 0; i < flex_struct->num; i++) {
        printf("%d ", flex_struct->data[i]);
    }
    printf("\n");

    /* COMPLIANT: Proper cleanup */
    free(flex_struct);
    return 0;
}