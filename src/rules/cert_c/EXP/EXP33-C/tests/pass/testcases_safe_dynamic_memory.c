/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Pass Case: safe_dynamic_memory.c
 *
 * This case demonstrates compliant dynamic memory allocation
 * and initialization patterns.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* COMPLIANT: Using calloc for zero-initialized memory */
void safe_calloc_usage(void) {
    int *numbers = calloc(10, sizeof(int));  /* Zero-initialized */

    if (numbers == NULL) {
        printf("Memory allocation failed\n");
        return;
    }

    /* All elements are guaranteed to be 0 */
    printf("Calloc initialized array: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    free(numbers);
}

/* COMPLIANT: Explicit initialization after malloc */
void safe_malloc_with_init(void) {
    int *array = malloc(10 * sizeof(int));

    if (array == NULL) {
        printf("Memory allocation failed\n");
        return;
    }

    /* Explicit initialization */
    for (int i = 0; i < 10; i++) {
        array[i] = i + 1;
    }

    printf("Explicitly initialized array: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    free(array);
}

/* COMPLIANT: Safe realloc with initialization of new memory */
void safe_realloc_usage(void) {
    int *array = malloc(5 * sizeof(int));
    if (array == NULL) return;

    /* Initialize original memory */
    for (int i = 0; i < 5; i++) {
        array[i] = i + 1;
    }

    /* Expand array */
    int *new_array = realloc(array, 10 * sizeof(int));
    if (new_array == NULL) {
        free(array);
        return;
    }
    array = new_array;

    /* Initialize newly allocated memory */
    for (int i = 5; i < 10; i++) {
        array[i] = 0;  /* Explicit initialization */
    }

    printf("Safe realloc array: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    free(array);
}

/* COMPLIANT: Safe string buffer allocation and initialization */
void safe_string_buffer_allocation(void) {
    size_t buffer_size = 256;
    char *buffer = malloc(buffer_size);

    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        return;
    }

    /* Initialize buffer before use */
    memset(buffer, 0, buffer_size);

    /* Safe string operations */
    strncpy(buffer, "Safely initialized string", buffer_size - 1);
    buffer[buffer_size - 1] = '\0';  /* Ensure null termination */

    printf("String buffer: %s\n", buffer);
    printf("Buffer length: %zu\n", strlen(buffer));

    free(buffer);
}

/* COMPLIANT: Safe 2D array allocation with initialization */
void safe_2d_array_allocation(void) {
    int rows = 3, cols = 4;
    int **matrix = malloc(rows * sizeof(int*));

    if (matrix == NULL) {
        printf("Matrix allocation failed\n");
        return;
    }

    /* Allocate and initialize each row */
    for (int i = 0; i < rows; i++) {
        matrix[i] = calloc(cols, sizeof(int));  /* Zero-initialized */
        if (matrix[i] == NULL) {
            /* Cleanup on failure */
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            printf("Row allocation failed\n");
            return;
        }
    }

    /* Set some values */
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = i * cols + j;
        }
    }

    /* Print matrix */
    printf("Safe 2D array:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%3d ", matrix[i][j]);
        }
        printf("\n");
    }

    /* Cleanup */
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

/* COMPLIANT: Safe structure allocation with initialization */
typedef struct {
    int id;
    char name[50];
    double value;
    int active;
} SafeData;

void safe_struct_allocation(void) {
    SafeData *data = malloc(sizeof(SafeData));

    if (data == NULL) {
        printf("Struct allocation failed\n");
        return;
    }

    /* Initialize all fields explicitly */
    data->id = 1001;
    strncpy(data->name, "Safe Structure", sizeof(data->name) - 1);
    data->name[sizeof(data->name) - 1] = '\0';
    data->value = 42.5;
    data->active = 1;

    printf("Safe struct - ID: %d, Name: %s, Value: %.1f, Active: %d\n",
           data->id, data->name, data->value, data->active);

    free(data);
}

/* COMPLIANT: Safe array of structures with calloc */
void safe_struct_array_allocation(void) {
    int count = 3;
    SafeData *array = calloc(count, sizeof(SafeData));  /* Zero-initialized */

    if (array == NULL) {
        printf("Struct array allocation failed\n");
        return;
    }

    /* Initialize each structure */
    for (int i = 0; i < count; i++) {
        array[i].id = 2000 + i;
        snprintf(array[i].name, sizeof(array[i].name), "Item_%d", i);
        array[i].value = (i + 1) * 10.5;
        array[i].active = 1;
    }

    printf("Safe struct array:\n");
    for (int i = 0; i < count; i++) {
        printf("  [%d] ID: %d, Name: %s, Value: %.1f, Active: %d\n",
               i, array[i].id, array[i].name, array[i].value, array[i].active);
    }

    free(array);
}

int main(void) {
    printf("=== Safe Dynamic Memory Demo ===\n");

    printf("1. Safe calloc usage:\n");
    safe_calloc_usage();

    printf("\n2. Safe malloc with initialization:\n");
    safe_malloc_with_init();

    printf("\n3. Safe realloc usage:\n");
    safe_realloc_usage();

    printf("\n4. Safe string buffer allocation:\n");
    safe_string_buffer_allocation();

    printf("\n5. Safe 2D array allocation:\n");
    safe_2d_array_allocation();

    printf("\n6. Safe struct allocation:\n");
    safe_struct_allocation();

    printf("\n7. Safe struct array allocation:\n");
    safe_struct_array_allocation();

    return 0;
}