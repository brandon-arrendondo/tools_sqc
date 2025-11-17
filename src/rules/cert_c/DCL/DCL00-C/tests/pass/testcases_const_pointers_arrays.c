/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Pass Case: const_pointers_arrays.c
 *
 * This case demonstrates compliant code that properly uses const
 * with pointers and arrays in various scenarios.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* COMPLIANT: Const pointer declarations at different levels */
void demonstrate_const_pointer_types(void) {
    /* COMPLIANT: Various const pointer configurations */
    const int value = 42;
    const int * const_ptr = &value;              /* Pointer to const int */
    const int * const const_ptr_to_const = &value; /* Const pointer to const int */

    /* COMPLIANT: Array that won't be modified */
    const int numbers[] = {1, 2, 3, 4, 5};
    const size_t numbers_size = sizeof(numbers) / sizeof(numbers[0]);

    /* COMPLIANT: Pointer to const array element */
    const int *ptr_to_const_element = &numbers[2];

    printf("Const Pointer Types Demonstration:\\n");
    printf("  Const value: %d\\n", value);
    printf("  Via const pointer: %d\\n", *const_ptr);
    printf("  Via const pointer to const: %d\\n", *const_ptr_to_const);
    printf("  Const array element [2]: %d\\n", *ptr_to_const_element);

    /* COMPLIANT: Iterating through const array */
    printf("  Complete const array: ");
    for (size_t i = 0; i < numbers_size; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\\n");
}

/* COMPLIANT: Function working with const string arrays */
void process_string_array(const char * const strings[], const size_t count) {
    /* COMPLIANT: Local const for processing */
    const char * const PROCESSING_MSG = "Processing string array";
    const char * const ITEM_FORMAT = "  [%zu]: '%s' (length: %zu)\\n";

    printf("\\n%s:\\n", PROCESSING_MSG);

    for (size_t i = 0; i < count; i++) {
        if (strings[i]) {
            const size_t str_length = strlen(strings[i]);
            printf(ITEM_FORMAT, i, strings[i], str_length);
        } else {
            printf("  [%zu]: (null)\\n", i);
        }
    }
}

/* COMPLIANT: Function demonstrating const array parameter */
double calculate_average(const double values[], const size_t count) {
    if (!values || count == 0) {
        return 0.0;
    }

    /* COMPLIANT: Local const for calculation */
    const char * const FUNCTION_NAME = "calculate_average";

    printf("\\n%s: Processing %zu values\\n", FUNCTION_NAME, count);

    double sum = 0.0;
    for (size_t i = 0; i < count; i++) {
        sum += values[i];
        printf("  Value [%zu]: %.2f\\n", i, values[i]);
    }

    const double average = sum / (double)count;
    printf("  Sum: %.2f, Average: %.2f\\n", sum, average);

    return average;
}

/* COMPLIANT: Function with const 2D array parameter */
void print_matrix(const int matrix[][3], const size_t rows) {
    /* COMPLIANT: Local const for dimensions and formatting */
    const size_t COLS = 3;
    const char * const MATRIX_HEADER = "Matrix content:";
    const char * const ROW_FORMAT = "  Row %zu: ";

    printf("\\n%s\\n", MATRIX_HEADER);

    for (size_t i = 0; i < rows; i++) {
        printf(ROW_FORMAT, i);
        for (size_t j = 0; j < COLS; j++) {
            printf("%4d ", matrix[i][j]);
        }
        printf("\\n");
    }
}

/* COMPLIANT: Function using const with dynamic memory */
char *create_const_aware_copy(const char *source) {
    if (!source) {
        return NULL;
    }

    /* COMPLIANT: Local const for string processing */
    const size_t source_length = strlen(source);
    const size_t buffer_size = source_length + 1;
    const char * const FUNCTION_NAME = "create_const_aware_copy";

    printf("\\n%s: Copying string of length %zu\\n", FUNCTION_NAME, source_length);

    char *copy = malloc(buffer_size);
    if (!copy) {
        printf("  Error: Memory allocation failed\\n");
        return NULL;
    }

    /* Copy the const source to new memory */
    strcpy(copy, source);

    printf("  Original: '%s'\\n", source);
    printf("  Copy created: '%s'\\n", copy);

    return copy;
}

/* COMPLIANT: Function demonstrating const with function pointers */
typedef int (*const_operation_t)(const int a, const int b);

int add_const_ints(const int a, const int b) {
    return a + b;
}

int multiply_const_ints(const int a, const int b) {
    return a * b;
}

void demonstrate_const_function_pointers(void) {
    /* COMPLIANT: Const array of function pointers */
    const const_operation_t operations[] = {
        add_const_ints,
        multiply_const_ints
    };
    const char * const operation_names[] = {
        "Addition",
        "Multiplication"
    };
    const size_t operation_count = sizeof(operations) / sizeof(operations[0]);

    /* COMPLIANT: Const test values */
    const int operand1 = 15;
    const int operand2 = 7;

    printf("\\nConst Function Pointers Demonstration:\\n");
    printf("  Operands: %d and %d\\n", operand1, operand2);

    for (size_t i = 0; i < operation_count; i++) {
        const int result = operations[i](operand1, operand2);
        printf("  %s: %d\\n", operation_names[i], result);
    }
}

/* COMPLIANT: Function with const structure containing arrays */
struct ConstDataSet {
    const char *name;
    const double *values;
    const size_t count;
};

void analyze_const_dataset(const struct ConstDataSet *dataset) {
    if (!dataset || !dataset->values || !dataset->name) {
        printf("Error: Invalid dataset\\n");
        return;
    }

    /* COMPLIANT: Local const for analysis */
    const char * const ANALYSIS_HEADER = "Dataset Analysis";
    const double average = calculate_average(dataset->values, dataset->count);

    printf("\\n%s:\\n", ANALYSIS_HEADER);
    printf("  Dataset name: %s\\n", dataset->name);
    printf("  Value count: %zu\\n", dataset->count);
    printf("  Average value: %.2f\\n", average);

    /* Find min and max using const data */
    if (dataset->count > 0) {
        const double *values = dataset->values;
        double min_val = values[0];
        double max_val = values[0];

        for (size_t i = 1; i < dataset->count; i++) {
            if (values[i] < min_val) min_val = values[i];
            if (values[i] > max_val) max_val = values[i];
        }

        printf("  Range: %.2f to %.2f\\n", min_val, max_val);
    }
}

int main(void) {
    /* COMPLIANT: Main function const declarations */
    const char * const PROGRAM_TITLE = "Const Pointers and Arrays Demo";
    const char * const SEPARATOR = "=====================================";

    printf("%s\\n", PROGRAM_TITLE);
    printf("%s\\n", SEPARATOR);

    /* Test basic const pointer types */
    demonstrate_const_pointer_types();

    /* Test const string array processing */
    const char * const test_strings[] = {
        "Hello",
        "World",
        "Programming",
        "Const",
        "Correctness"
    };
    const size_t string_count = sizeof(test_strings) / sizeof(test_strings[0]);
    process_string_array(test_strings, string_count);

    /* Test const numeric array processing */
    const double test_values[] = {1.5, 2.7, 3.14159, 4.0, 5.5};
    const size_t values_count = sizeof(test_values) / sizeof(test_values[0]);
    calculate_average(test_values, values_count);

    /* Test const 2D array */
    const int test_matrix[][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    const size_t matrix_rows = sizeof(test_matrix) / sizeof(test_matrix[0]);
    print_matrix(test_matrix, matrix_rows);

    /* Test const-aware string copying */
    const char * const original_string = "This is a test string for copying";
    char *copied_string = create_const_aware_copy(original_string);
    if (copied_string) {
        printf("  Memory management successful\\n");
        free(copied_string);
    }

    /* Test const function pointers */
    demonstrate_const_function_pointers();

    /* Test const dataset analysis */
    const double dataset_values[] = {10.5, 15.2, 8.7, 22.1, 18.9, 12.3};
    const struct ConstDataSet dataset = {
        "Sample Measurements",
        dataset_values,
        sizeof(dataset_values) / sizeof(dataset_values[0])
    };
    analyze_const_dataset(&dataset);

    printf("\\n%s\\n", SEPARATOR);
    printf("Demo completed successfully\\n");

    return 0;
}