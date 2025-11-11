/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Pass Case: struct_copying_function.c
 *
 * This case demonstrates compliant code that implements a proper
 * copying function for structures with flexible array members,
 * handling all aspects of memory allocation and data copying.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

/* COMPLIANT: Function to properly copy a flexible array structure */
struct flex_array_struct *copy_flex_struct(const struct flex_array_struct *src) {
    struct flex_array_struct *dest;
    size_t total_size;

    if (src == NULL) return NULL;

    /* Calculate total size needed */
    total_size = sizeof(struct flex_array_struct) + sizeof(int) * src->num;

    /* Allocate memory for the copy */
    dest = malloc(total_size);
    if (dest == NULL) return NULL;

    /* COMPLIANT: Copy using memcpy with correct size */
    memcpy(dest, src, total_size);

    return dest;
}

/* COMPLIANT: Function to compare two flexible array structures */
int compare_flex_structs(const struct flex_array_struct *a,
                        const struct flex_array_struct *b) {
    if (a == NULL && b == NULL) return 1;  /* Both NULL, considered equal */
    if (a == NULL || b == NULL) return 0;  /* One NULL, not equal */

    if (a->num != b->num) return 0;  /* Different sizes */

    /* Compare the data arrays */
    for (size_t i = 0; i < a->num; i++) {
        if (a->data[i] != b->data[i]) return 0;
    }

    return 1;  /* All elements match */
}

/* COMPLIANT: Function to create and initialize a flexible array structure */
struct flex_array_struct *create_initialized_struct(size_t size, int base_value) {
    struct flex_array_struct *new_struct;

    if (size == 0) return NULL;

    new_struct = malloc(sizeof(struct flex_array_struct) + sizeof(int) * size);
    if (new_struct == NULL) return NULL;

    new_struct->num = size;
    for (size_t i = 0; i < size; i++) {
        new_struct->data[i] = base_value + (int)i;
    }

    return new_struct;
}

int main(void) {
    struct flex_array_struct *original, *copy1, *copy2;

    /* Create original structure */
    original = create_initialized_struct(4, 100);
    if (original == NULL) {
        fprintf(stderr, "Failed to create original structure\n");
        return 1;
    }

    printf("Original structure:\n");
    printf("num: %zu, data: ", original->num);
    for (size_t i = 0; i < original->num; i++) {
        printf("%d ", original->data[i]);
    }
    printf("\n");

    /* COMPLIANT: Create copy using proper copying function */
    copy1 = copy_flex_struct(original);
    if (copy1 == NULL) {
        fprintf(stderr, "Failed to copy structure\n");
        free(original);
        return 1;
    }

    printf("\nCopy 1:\n");
    printf("num: %zu, data: ", copy1->num);
    for (size_t i = 0; i < copy1->num; i++) {
        printf("%d ", copy1->data[i]);
    }
    printf("\n");

    /* Verify they are equal but separate */
    printf("Original and copy1 are %s\n",
           compare_flex_structs(original, copy1) ? "equal" : "different");
    printf("Original and copy1 have %s addresses\n",
           (original == copy1) ? "same" : "different");

    /* Modify copy to show independence */
    copy1->data[0] = 999;

    printf("\nAfter modifying copy1[0] = 999:\n");
    printf("Original[0]: %d\n", original->data[0]);
    printf("Copy1[0]: %d\n", copy1->data[0]);
    printf("Structures are now %s\n",
           compare_flex_structs(original, copy1) ? "equal" : "different");

    /* Create another copy from original */
    copy2 = copy_flex_struct(original);
    if (copy2 != NULL) {
        printf("\nCopy2 from original:\n");
        printf("num: %zu, data: ", copy2->num);
        for (size_t i = 0; i < copy2->num; i++) {
            printf("%d ", copy2->data[i]);
        }
        printf("\n");

        printf("Original and copy2 are %s\n",
               compare_flex_structs(original, copy2) ? "equal" : "different");
    }

    /* COMPLIANT: Proper cleanup */
    free(original);
    free(copy1);
    if (copy2 != NULL) free(copy2);

    return 0;
}