/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Pass Case: pass_by_pointer.c
 *
 * This case demonstrates compliant code that properly passes structures
 * containing flexible array members by pointer to functions, preserving
 * access to the complete flexible array data.
 */

#include <stdio.h>
#include <stdlib.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

/* COMPLIANT: Function accepts flexible array struct by pointer */
void print_flex_struct(const struct flex_array_struct *flex) {
    if (flex == NULL) {
        printf("NULL pointer passed\n");
        return;
    }

    printf("Structure contains %zu elements:\n", flex->num);
    for (size_t i = 0; i < flex->num; i++) {
        printf("  data[%zu] = %d\n", i, flex->data[i]);
    }
}

/* COMPLIANT: Function modifies flexible array struct through pointer */
void double_values(struct flex_array_struct *flex) {
    if (flex == NULL) return;

    printf("Doubling all values...\n");
    for (size_t i = 0; i < flex->num; i++) {
        flex->data[i] *= 2;
    }
}

/* COMPLIANT: Function returns pointer to new flexible array struct */
struct flex_array_struct *create_flex_struct(size_t size, int initial_value) {
    struct flex_array_struct *new_struct;

    new_struct = malloc(sizeof(struct flex_array_struct) + sizeof(int) * size);
    if (new_struct == NULL) return NULL;

    new_struct->num = size;
    for (size_t i = 0; i < size; i++) {
        new_struct->data[i] = initial_value + (int)i;
    }

    return new_struct;
}

int main(void) {
    struct flex_array_struct *flex_struct;

    /* COMPLIANT: Create structure using function that returns pointer */
    flex_struct = create_flex_struct(4, 50);
    if (flex_struct == NULL) {
        fprintf(stderr, "Failed to create structure\n");
        return 1;
    }

    printf("Original structure:\n");
    print_flex_struct(flex_struct);

    /* COMPLIANT: Modify through pointer */
    double_values(flex_struct);

    printf("\nAfter doubling:\n");
    print_flex_struct(flex_struct);

    /* COMPLIANT: Proper cleanup */
    free(flex_struct);
    return 0;
}