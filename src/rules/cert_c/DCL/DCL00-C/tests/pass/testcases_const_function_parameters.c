/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Pass Case: const_function_parameters.c
 *
 * This case demonstrates compliant code that properly uses const
 * qualifiers for function parameters, improving API clarity and safety.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* COMPLIANT: Function with const parameters for input data */
void print_array(const int *array, const size_t size, const char *label) {
    if (!array || !label) {
        printf("Error: NULL parameter\\n");
        return;
    }

    printf("%s: ", label);
    for (size_t i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\\n");
}

/* COMPLIANT: Function with const string parameter */
size_t count_vowels(const char *text) {
    if (!text) {
        return 0;
    }

    /* COMPLIANT: Local const string for lookup */
    const char * const VOWELS = "aeiouAEIOU";
    size_t count = 0;

    for (size_t i = 0; text[i] != '\\0'; i++) {
        if (strchr(VOWELS, text[i]) != NULL) {
            count++;
        }
    }

    return count;
}

/* COMPLIANT: Function with const struct parameter */
struct Point {
    double x;
    double y;
};

double calculate_distance(const struct Point *p1, const struct Point *p2) {
    if (!p1 || !p2) {
        return -1.0;  /* Error indicator */
    }

    const double dx = p2->x - p1->x;
    const double dy = p2->y - p1->y;
    const double distance = sqrt(dx * dx + dy * dy);

    return distance;
}

/* COMPLIANT: Function with const array parameter and const local variables */
int find_maximum(const int *values, const size_t count) {
    if (!values || count == 0) {
        return INT_MIN;  /* Error indicator */
    }

    int max_value = values[0];
    const char * const FUNCTION_NAME = "find_maximum";

    printf("  %s: Processing %zu values\\n", FUNCTION_NAME, count);

    for (size_t i = 1; i < count; i++) {
        if (values[i] > max_value) {
            max_value = values[i];
        }
    }

    return max_value;
}

/* COMPLIANT: Function with multiple const parameters */
char *create_formatted_string(const char *template, const char *value1,
                             const int value2, const double value3) {
    if (!template || !value1) {
        return NULL;
    }

    /* COMPLIANT: Const calculation for buffer size */
    const size_t TEMPLATE_LEN = strlen(template);
    const size_t VALUE1_LEN = strlen(value1);
    const size_t EXTRA_SPACE = 100;  /* For numbers and formatting */
    const size_t BUFFER_SIZE = TEMPLATE_LEN + VALUE1_LEN + EXTRA_SPACE;

    char *result = malloc(BUFFER_SIZE);
    if (!result) {
        return NULL;
    }

    snprintf(result, BUFFER_SIZE, template, value1, value2, value3);
    return result;
}

/* COMPLIANT: Function using const for read-only data processing */
void analyze_text(const char *text) {
    if (!text) {
        printf("Error: NULL text provided\\n");
        return;
    }

    /* COMPLIANT: Local const variables for analysis */
    const size_t TEXT_LENGTH = strlen(text);
    const char * const FUNCTION_NAME = "analyze_text";

    printf("\\n%s Analysis:\\n", FUNCTION_NAME);
    printf("  Text: '%s'\\n", text);
    printf("  Length: %zu characters\\n", TEXT_LENGTH);

    /* Count different character types using const parameters */
    size_t vowel_count = count_vowels(text);
    printf("  Vowels: %zu\\n", vowel_count);

    /* Count other characteristics */
    size_t space_count = 0;
    size_t digit_count = 0;
    size_t alpha_count = 0;

    for (size_t i = 0; i < TEXT_LENGTH; i++) {
        const char c = text[i];
        if (c == ' ') space_count++;
        else if (c >= '0' && c <= '9') digit_count++;
        else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) alpha_count++;
    }

    printf("  Spaces: %zu\\n", space_count);
    printf("  Digits: %zu\\n", digit_count);
    printf("  Alphabetic: %zu\\n", alpha_count);
}

/* COMPLIANT: Function with const pointer to const data */
void display_configuration(const char * const * const config_keys,
                          const char * const * const config_values,
                          const size_t config_count) {
    if (!config_keys || !config_values) {
        printf("Error: NULL configuration data\\n");
        return;
    }

    /* COMPLIANT: Local const for formatting */
    const char * const HEADER_FORMAT = "Configuration (%zu items):\\n";
    const char * const ITEM_FORMAT = "  %-15s = %s\\n";

    printf(HEADER_FORMAT, config_count);

    for (size_t i = 0; i < config_count; i++) {
        if (config_keys[i] && config_values[i]) {
            printf(ITEM_FORMAT, config_keys[i], config_values[i]);
        }
    }
}

/* COMPLIANT: Function demonstrating const correctness with structures */
struct Rectangle {
    double width;
    double height;
};

void print_rectangle_info(const struct Rectangle *rect, const char *name) {
    if (!rect || !name) {
        printf("Error: NULL parameter\\n");
        return;
    }

    /* COMPLIANT: Local const calculations */
    const double area = rect->width * rect->height;
    const double perimeter = 2.0 * (rect->width + rect->height);
    const char * const UNIT = "units";

    printf("\\nRectangle '%s':\\n", name);
    printf("  Dimensions: %.2f x %.2f %s\\n", rect->width, rect->height, UNIT);
    printf("  Area: %.2f square %s\\n", area, UNIT);
    printf("  Perimeter: %.2f %s\\n", perimeter, UNIT);
}

/* COMPLIANT: Function with const callback parameter */
typedef int (*comparison_func_t)(const void *a, const void *b);

void sort_demonstration(int *array, const size_t size,
                       const comparison_func_t compare) {
    if (!array || !compare || size == 0) {
        return;
    }

    /* COMPLIANT: Local const for algorithm description */
    const char * const ALGORITHM_NAME = "Simple Bubble Sort";

    printf("\\nSorting with %s:\\n", ALGORITHM_NAME);
    print_array(array, size, "Before");

    /* Simple bubble sort implementation */
    for (size_t i = 0; i < size - 1; i++) {
        for (size_t j = 0; j < size - i - 1; j++) {
            if (compare(&array[j], &array[j + 1]) > 0) {
                /* Swap elements */
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }

    print_array(array, size, "After");
}

/* COMPLIANT: Comparison function with const parameters */
int compare_integers(const void *a, const void *b) {
    const int *ia = (const int *)a;
    const int *ib = (const int *)b;
    return (*ia > *ib) - (*ia < *ib);
}

int main(void) {
    /* COMPLIANT: Main function const declarations */
    const char * const PROGRAM_TITLE = "Const Function Parameters Demo";
    const char * const SEPARATOR = "=====================================";

    printf("%s\\n", PROGRAM_TITLE);
    printf("%s\\n", SEPARATOR);

    /* Test array functions */
    const int test_array[] = {64, 34, 25, 12, 22, 11, 90};
    const size_t array_size = sizeof(test_array) / sizeof(test_array[0]);

    print_array(test_array, array_size, "Test Array");

    int max_value = find_maximum(test_array, array_size);
    printf("  Maximum value: %d\\n", max_value);

    /* Test string functions */
    const char * const test_text = "Hello Programming World!";
    analyze_text(test_text);

    /* Test formatted string creation */
    const char * const template = "Name: %s, Count: %d, Average: %.2f";
    char *formatted = create_formatted_string(template, "TestData", 42, 3.14159);
    if (formatted) {
        printf("\\nFormatted string: %s\\n", formatted);
        free(formatted);
    }

    /* Test configuration display */
    const char * const config_keys[] = {"host", "port", "timeout", "retries"};
    const char * const config_values[] = {"localhost", "8080", "30", "3"};
    const size_t config_count = sizeof(config_keys) / sizeof(config_keys[0]);

    display_configuration(config_keys, config_values, config_count);

    /* Test geometry functions */
    const struct Rectangle rect = {10.5, 7.2};
    print_rectangle_info(&rect, "Test Rectangle");

    const struct Point p1 = {0.0, 0.0};
    const struct Point p2 = {3.0, 4.0};
    double distance = calculate_distance(&p1, &p2);
    printf("\\nDistance between points: %.2f\\n", distance);

    /* Test sorting with const callback */
    int sort_array[] = {64, 34, 25, 12, 22, 11, 90};
    const size_t sort_size = sizeof(sort_array) / sizeof(sort_array[0]);
    sort_demonstration(sort_array, sort_size, compare_integers);

    printf("\\n%s\\n", SEPARATOR);
    printf("Demo completed successfully\\n");

    return 0;
}