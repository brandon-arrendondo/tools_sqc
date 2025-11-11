/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Pass Case: realloc_proper_size.c
 *
 * This case demonstrates compliant code that properly uses realloc()
 * to resize a structure containing a flexible array member with
 * correct size calculations.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

int main(void) {
    struct flex_array_struct *flex_struct;
    size_t initial_size = 3;
    size_t new_size = 6;

    /* COMPLIANT: Initial proper allocation */
    flex_struct = malloc(sizeof(struct flex_array_struct) + sizeof(int) * initial_size);
    if (flex_struct == NULL) return 1;

    flex_struct->num = initial_size;
    for (size_t i = 0; i < initial_size; i++) {
        flex_struct->data[i] = (int)(i + 1);
    }

    printf("Initial structure:\n");
    printf("Size: %zu\n", flex_struct->num);
    printf("Data: ");
    for (size_t i = 0; i < flex_struct->num; i++) {
        printf("%d ", flex_struct->data[i]);
    }
    printf("\n");

    /* COMPLIANT: Proper realloc with correct size calculation */
    size_t new_total_size = sizeof(struct flex_array_struct) + sizeof(int) * new_size;
    struct flex_array_struct *temp = realloc(flex_struct, new_total_size);

    if (temp == NULL) {
        fprintf(stderr, "Realloc failed\n");
        free(flex_struct);
        return 1;
    }

    flex_struct = temp;

    /* Update size and initialize new elements */
    flex_struct->num = new_size;
    for (size_t i = initial_size; i < new_size; i++) {
        flex_struct->data[i] = (int)(i + 1);
    }

    printf("\nAfter realloc:\n");
    printf("Size: %zu\n", flex_struct->num);
    printf("Data: ");
    for (size_t i = 0; i < flex_struct->num; i++) {
        printf("%d ", flex_struct->data[i]);
    }
    printf("\n");

    /* COMPLIANT: Shrink the array */
    size_t final_size = 4;
    size_t final_total_size = sizeof(struct flex_array_struct) + sizeof(int) * final_size;

    temp = realloc(flex_struct, final_total_size);
    if (temp != NULL) {
        flex_struct = temp;
        flex_struct->num = final_size;

        printf("\nAfter shrinking:\n");
        printf("Size: %zu\n", flex_struct->num);
        printf("Data: ");
        for (size_t i = 0; i < flex_struct->num; i++) {
            printf("%d ", flex_struct->data[i]);
        }
        printf("\n");
    }

    /* COMPLIANT: Proper cleanup */
    free(flex_struct);
    return 0;
}