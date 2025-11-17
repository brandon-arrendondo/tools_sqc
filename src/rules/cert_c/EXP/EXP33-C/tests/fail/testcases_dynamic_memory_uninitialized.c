/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: dynamic_memory_uninitialized.c
 *
 * This case demonstrates violations involving uninitialized
 * dynamically allocated memory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Reading uninitialized malloc'd memory */
void unsafe_malloc_usage(void) {
    int *numbers = malloc(10 * sizeof(int));  /* Uninitialized memory */

    if (numbers == NULL) {
        return;
    }

    /* Reading uninitialized memory */
    printf("First number: %d\n", numbers[0]);  /* Undefined behavior */

    /* Using uninitialized data in calculations */
    int sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += numbers[i];  /* Undefined behavior */
    }
    printf("Sum: %d\n", sum);

    free(numbers);
}

/* NON-COMPLIANT: Realloc with uninitialized expanded memory */
void unsafe_realloc_usage(void) {
    int *array = malloc(5 * sizeof(int));
    if (array == NULL) return;

    /* Initialize only original memory */
    for (int i = 0; i < 5; i++) {
        array[i] = i + 1;
    }

    /* Expand array */
    array = realloc(array, 10 * sizeof(int));
    if (array == NULL) return;

    /* Reading expanded uninitialized memory */
    printf("Array contents:\n");
    for (int i = 0; i < 10; i++) {
        printf("array[%d] = %d\n", i, array[i]);  /* Undefined behavior for i >= 5 */
    }

    free(array);
}

/* NON-COMPLIANT: Uninitialized buffer for string operations */
void unsafe_string_buffer(void) {
    char *buffer = malloc(256);  /* Uninitialized memory */
    if (buffer == NULL) return;

    /* Trying to use buffer as string without initialization */
    int len = strlen(buffer);  /* Undefined behavior */
    printf("Buffer length: %d\n", len);

    /* Appending to uninitialized buffer */
    strcat(buffer, " - appended");  /* Undefined behavior */
    printf("Buffer: %s\n", buffer);

    free(buffer);
}

/* NON-COMPLIANT: Dynamic 2D array allocation */
void unsafe_2d_array(void) {
    int rows = 3, cols = 4;
    int **matrix = malloc(rows * sizeof(int*));
    if (matrix == NULL) return;

    for (int i = 0; i < rows; i++) {
        matrix[i] = malloc(cols * sizeof(int));  /* Uninitialized memory */
        if (matrix[i] == NULL) {
            /* Cleanup and return */
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            return;
        }
    }

    /* Reading uninitialized 2D array */
    printf("Matrix contents:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);  /* Undefined behavior */
        }
        printf("\n");
    }

    /* Cleanup */
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int main(void) {
    printf("=== Dynamic Memory Uninitialized Demo ===\n");

    printf("1. Unsafe malloc usage:\n");
    unsafe_malloc_usage();

    printf("\n2. Unsafe realloc usage:\n");
    unsafe_realloc_usage();

    printf("\n3. Unsafe string buffer:\n");
    unsafe_string_buffer();

    printf("\n4. Unsafe 2D array:\n");
    unsafe_2d_array();

    return 0;
}