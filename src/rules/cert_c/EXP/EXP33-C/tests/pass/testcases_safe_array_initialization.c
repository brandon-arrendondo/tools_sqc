/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Pass Case: safe_array_initialization.c
 *
 * This case demonstrates compliant array initialization patterns
 * that ensure all elements are properly initialized.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* COMPLIANT: Complete array initialization */
void safe_array_operations(void) {
    /* Zero-initialize entire array */
    int numbers[10] = {0};

    /* Partial initialization with remainder zeroed */
    int values[5] = {1, 2, 3};  /* values[3] and values[4] are 0 */

    /* Explicit complete initialization */
    int sequence[6] = {1, 2, 3, 4, 5, 6};

    printf("Zero-initialized array: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    printf("Partially initialized array: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", values[i]);
    }
    printf("\n");

    printf("Fully initialized sequence: ");
    for (int i = 0; i < 6; i++) {
        printf("%d ", sequence[i]);
    }
    printf("\n");
}

/* COMPLIANT: Safe character array initialization */
void safe_character_arrays(void) {
    /* Zero-initialize character array */
    char buffer[256] = {0};

    /* String literal initialization (automatically null-terminated) */
    char message[] = "Hello, World!";

    /* Explicit initialization with size */
    char greeting[20] = "Hello";  /* Remainder is zero-initialized */

    /* Safe string operations */
    strncpy(buffer, "Safe string copy", sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';  /* Ensure null termination */

    printf("Buffer: %s\n", buffer);
    printf("Message: %s\n", message);
    printf("Greeting: %s\n", greeting);
    printf("Buffer length: %zu\n", strlen(buffer));
}

/* COMPLIANT: Multidimensional array initialization */
void safe_multidimensional_arrays(void) {
    /* Complete 2D array initialization */
    int matrix[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    /* Partial initialization with remainder zeroed */
    int sparse[4][4] = {
        {1, 0, 0, 0},
        {0, 2, 0, 0}
        /* Remaining rows are zero-initialized */
    };

    /* Zero-initialize entire 2D array */
    int zeros[2][5] = {0};

    printf("Complete matrix:\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    printf("\nSparse matrix:\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", sparse[i][j]);
        }
        printf("\n");
    }

    printf("\nZero matrix:\n");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%d ", zeros[i][j]);
        }
        printf("\n");
    }
}

/* COMPLIANT: Safe variable-length array handling */
void safe_vla_usage(int size) {
    if (size <= 0 || size > 1000) {  /* Validate size */
        printf("Invalid VLA size: %d\n", size);
        return;
    }

    int vla[size];

    /* Initialize all elements explicitly */
    for (int i = 0; i < size; i++) {
        vla[i] = i * i;  /* Initialize with square values */
    }

    /* Safe access to fully initialized VLA */
    printf("VLA contents (size %d): ", size);
    for (int i = 0; i < size; i++) {
        printf("%d ", vla[i]);
    }
    printf("\n");
}

/* COMPLIANT: Safe array parameter processing */
void safe_process_array(int array[], int size) {
    if (array == NULL || size <= 0) {
        printf("Invalid array parameters\n");
        return;
    }

    int sum = 0;
    int max = array[0];  /* Safe since we validated size > 0 */
    int min = array[0];

    for (int i = 0; i < size; i++) {
        sum += array[i];
        if (array[i] > max) max = array[i];
        if (array[i] < min) min = array[i];
    }

    printf("Array stats - Sum: %d, Max: %d, Min: %d\n", sum, max, min);
}

void test_safe_array_processing(void) {
    /* Properly initialized arrays */
    int data1[] = {5, 2, 8, 1, 9, 3};
    int data2[5] = {10, 20, 30, 40, 50};

    printf("Processing data1:\n");
    safe_process_array(data1, sizeof(data1) / sizeof(data1[0]));

    printf("Processing data2:\n");
    safe_process_array(data2, 5);
}

/* COMPLIANT: Safe string array initialization */
void safe_string_arrays(void) {
    /* Array of string literals */
    const char *fruits[] = {
        "apple",
        "banana",
        "cherry",
        "date",
        "elderberry"
    };
    int fruit_count = sizeof(fruits) / sizeof(fruits[0]);

    /* Character array for strings with explicit initialization */
    char colors[][10] = {
        "red",
        "green",
        "blue",
        "yellow"
    };
    int color_count = sizeof(colors) / sizeof(colors[0]);

    printf("Fruits:\n");
    for (int i = 0; i < fruit_count; i++) {
        printf("  %d: %s\n", i, fruits[i]);
    }

    printf("\nColors:\n");
    for (int i = 0; i < color_count; i++) {
        printf("  %d: %s\n", i, colors[i]);
    }
}

/* COMPLIANT: Safe array copying and manipulation */
void safe_array_operations_advanced(void) {
    /* Source array properly initialized */
    int source[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int destination[10] = {0};  /* Zero-initialized destination */

    /* Safe array copying */
    memcpy(destination, source, sizeof(source));

    /* Verify copy */
    printf("Array copy verification:\n");
    printf("Source:      ");
    for (int i = 0; i < 10; i++) {
        printf("%2d ", source[i]);
    }
    printf("\n");

    printf("Destination: ");
    for (int i = 0; i < 10; i++) {
        printf("%2d ", destination[i]);
    }
    printf("\n");

    /* Safe array modification */
    for (int i = 0; i < 10; i++) {
        destination[i] *= 2;  /* Double all values */
    }

    printf("Modified:    ");
    for (int i = 0; i < 10; i++) {
        printf("%2d ", destination[i]);
    }
    printf("\n");
}

int main(void) {
    printf("=== Safe Array Initialization Demo ===\n");

    printf("1. Basic array operations:\n");
    safe_array_operations();

    printf("\n2. Character arrays:\n");
    safe_character_arrays();

    printf("\n3. Multidimensional arrays:\n");
    safe_multidimensional_arrays();

    printf("\n4. Variable-length arrays:\n");
    safe_vla_usage(5);

    printf("\n5. Array parameter processing:\n");
    test_safe_array_processing();

    printf("\n6. String arrays:\n");
    safe_string_arrays();

    printf("\n7. Advanced array operations:\n");
    safe_array_operations_advanced();

    return 0;
}