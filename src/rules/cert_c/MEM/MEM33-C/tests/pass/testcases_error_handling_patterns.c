/*
 * Rule: MEM33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MEM33-C violation
 */

/*
 * CERT C MEM33-C Pass Case: error_handling_patterns.c
 *
 * This case demonstrates compliant code that implements robust error
 * handling patterns when working with structures containing flexible
 * array members, including proper cleanup on allocation failures.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct flex_array_struct {
    size_t num;
    int data[];  /* Flexible array member */
};

/* COMPLIANT: Function with proper error handling */
struct flex_array_struct *safe_create_flex_struct(size_t size, int *error_code) {
    struct flex_array_struct *new_struct;
    size_t total_size;

    /* Initialize error code */
    if (error_code != NULL) {
        *error_code = 0;
    }

    /* Validate input */
    if (size == 0) {
        if (error_code != NULL) {
            *error_code = EINVAL;
        }
        return NULL;
    }

    /* Check for potential overflow in size calculation */
    if (size > (SIZE_MAX - sizeof(struct flex_array_struct)) / sizeof(int)) {
        if (error_code != NULL) {
            *error_code = EOVERFLOW;
        }
        return NULL;
    }

    total_size = sizeof(struct flex_array_struct) + sizeof(int) * size;

    /* COMPLIANT: Proper allocation with error checking */
    new_struct = malloc(total_size);
    if (new_struct == NULL) {
        if (error_code != NULL) {
            *error_code = ENOMEM;
        }
        return NULL;
    }

    /* Initialize the structure */
    new_struct->num = size;
    for (size_t i = 0; i < size; i++) {
        new_struct->data[i] = 0;  /* Safe default value */
    }

    return new_struct;
}

/* COMPLIANT: Function to safely resize a flexible array structure */
struct flex_array_struct *safe_resize_flex_struct(struct flex_array_struct *original,
                                                 size_t new_size, int *error_code) {
    struct flex_array_struct *resized;
    size_t copy_size, new_total_size;

    if (error_code != NULL) {
        *error_code = 0;
    }

    if (original == NULL) {
        if (error_code != NULL) {
            *error_code = EINVAL;
        }
        return NULL;
    }

    if (new_size == 0) {
        if (error_code != NULL) {
            *error_code = EINVAL;
        }
        return NULL;
    }

    /* Check for overflow */
    if (new_size > (SIZE_MAX - sizeof(struct flex_array_struct)) / sizeof(int)) {
        if (error_code != NULL) {
            *error_code = EOVERFLOW;
        }
        return NULL;
    }

    new_total_size = sizeof(struct flex_array_struct) + sizeof(int) * new_size;

    /* COMPLIANT: Use realloc with proper error handling */
    resized = realloc(original, new_total_size);
    if (resized == NULL) {
        if (error_code != NULL) {
            *error_code = ENOMEM;
        }
        /* original pointer is still valid when realloc fails */
        return NULL;
    }

    /* Initialize new elements if expanding */
    if (new_size > resized->num) {
        for (size_t i = resized->num; i < new_size; i++) {
            resized->data[i] = 0;
        }
    }

    resized->num = new_size;
    return resized;
}

/* COMPLIANT: Safe cleanup function */
void safe_free_flex_struct(struct flex_array_struct **flex_ptr) {
    if (flex_ptr != NULL && *flex_ptr != NULL) {
        free(*flex_ptr);
        *flex_ptr = NULL;  /* Prevent double-free */
    }
}

int main(void) {
    struct flex_array_struct *flex_struct = NULL;
    int error_code;

    printf("Testing error handling patterns:\n\n");

    /* Test 1: Invalid size */
    printf("Test 1: Creating structure with size 0\n");
    flex_struct = safe_create_flex_struct(0, &error_code);
    if (flex_struct == NULL) {
        printf("Expected failure: error_code = %d (EINVAL = %d)\n", error_code, EINVAL);
    }

    /* Test 2: Valid creation */
    printf("\nTest 2: Creating structure with size 3\n");
    flex_struct = safe_create_flex_struct(3, &error_code);
    if (flex_struct != NULL) {
        printf("Success: Created structure with %zu elements\n", flex_struct->num);

        /* Initialize with test data */
        for (size_t i = 0; i < flex_struct->num; i++) {
            flex_struct->data[i] = (int)(i + 1);
        }

        printf("Initial data: ");
        for (size_t i = 0; i < flex_struct->num; i++) {
            printf("%d ", flex_struct->data[i]);
        }
        printf("\n");
    } else {
        printf("Unexpected failure: error_code = %d\n", error_code);
        return 1;
    }

    /* Test 3: Successful resize */
    printf("\nTest 3: Resizing to 5 elements\n");
    struct flex_array_struct *resized = safe_resize_flex_struct(flex_struct, 5, &error_code);
    if (resized != NULL) {
        flex_struct = resized;  /* Update pointer */
        printf("Success: Resized to %zu elements\n", flex_struct->num);

        printf("Data after resize: ");
        for (size_t i = 0; i < flex_struct->num; i++) {
            printf("%d ", flex_struct->data[i]);
        }
        printf("\n");
    } else {
        printf("Resize failed: error_code = %d\n", error_code);
    }

    /* Test 4: Resize to smaller size */
    printf("\nTest 4: Resizing to 2 elements\n");
    resized = safe_resize_flex_struct(flex_struct, 2, &error_code);
    if (resized != NULL) {
        flex_struct = resized;
        printf("Success: Resized to %zu elements\n", flex_struct->num);

        printf("Data after shrinking: ");
        for (size_t i = 0; i < flex_struct->num; i++) {
            printf("%d ", flex_struct->data[i]);
        }
        printf("\n");
    } else {
        printf("Resize failed: error_code = %d\n", error_code);
    }

    /* Test 5: Attempt to resize with invalid size */
    printf("\nTest 5: Attempting to resize to 0 elements\n");
    resized = safe_resize_flex_struct(flex_struct, 0, &error_code);
    if (resized == NULL) {
        printf("Expected failure: error_code = %d (EINVAL = %d)\n", error_code, EINVAL);
        printf("Original structure preserved\n");
    }

    /* COMPLIANT: Safe cleanup */
    printf("\nTest 6: Safe cleanup\n");
    safe_free_flex_struct(&flex_struct);
    printf("Structure freed, pointer set to NULL\n");

    /* Test double-free protection */
    printf("Testing double-free protection: ");
    safe_free_flex_struct(&flex_struct);
    printf("No crash - double-free prevented\n");

    return 0;
}