/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Pass Case: safe_function_interfaces.c
 *
 * This case demonstrates compliant function interfaces that
 * properly initialize output parameters and handle all code paths.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* COMPLIANT: Function that initializes output parameter in all paths */
int safe_get_user_input(int *value) {
    if (value == NULL) {
        return -1;  /* Invalid parameter */
    }

    *value = 0;  /* Initialize output parameter immediately */

    char input[100];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return -1;  /* value is still initialized to 0 */
    }

    if (strlen(input) == 0) {
        return -1;  /* value remains initialized to 0 */
    }

    *value = atoi(input);
    return 0;
}

/* COMPLIANT: Function with proper parameter validation and initialization */
void safe_process_coordinates(int x, int y, int z) {
    /* All parameters are passed by value, so they're guaranteed initialized */
    printf("Processing point (%d, %d, %d)\n", x, y, z);

    double distance = sqrt(x*x + y*y + z*z);
    printf("Distance from origin: %.2f\n", distance);
}

void safe_coordinate_usage(void) {
    /* Initialize all variables before passing */
    int x = 10, y = 20, z = 30;

    safe_process_coordinates(x, y, z);
}

/* COMPLIANT: Function that always returns valid value */
int safe_calculate_result(int a, int b, int operation) {
    int result = 0;  /* Default initialization */

    switch (operation) {
        case 1:
            result = a + b;
            break;
        case 2:
            result = a - b;
            break;
        case 3:
            result = a * b;
            break;
        case 4:
            if (b != 0) {
                result = a / b;
            } else {
                result = 0;  /* Safe default for division by zero */
            }
            break;
        default:
            result = 0;  /* Safe default for unknown operations */
            break;
    }

    return result;  /* Always returns initialized value */
}

/* COMPLIANT: Safe callback function usage */
void safe_process_item(int item, void (*callback)(int)) {
    if (callback != NULL) {
        callback(item);
    }
}

void safe_callback_function(int value) {
    printf("Callback received: %d\n", value);
}

void safe_callback_usage(void) {
    int data = 42;  /* Properly initialized */

    safe_process_item(data, safe_callback_function);
}

/* COMPLIANT: Safe variadic function with proper argument handling */
void safe_print_numbers(int count, ...) {
    if (count <= 0) {
        printf("No numbers to print\n");
        return;
    }

    va_list args;
    va_start(args, count);

    printf("Numbers: ");
    for (int i = 0; i < count; i++) {
        int num = va_arg(args, int);
        printf("%d ", num);
    }
    printf("\n");

    va_end(args);
}

void safe_variadic_usage(void) {
    int a = 1, b = 2, c = 3;  /* All initialized */

    safe_print_numbers(3, a, b, c);
}

/* COMPLIANT: Function with multiple output parameters */
int safe_divide_with_remainder(int dividend, int divisor, int *quotient, int *remainder) {
    /* Validate input parameters */
    if (quotient == NULL || remainder == NULL) {
        return -1;  /* Invalid output parameters */
    }

    if (divisor == 0) {
        *quotient = 0;   /* Initialize outputs even on error */
        *remainder = 0;
        return -1;  /* Division by zero */
    }

    /* Perform calculation and set outputs */
    *quotient = dividend / divisor;
    *remainder = dividend % divisor;

    return 0;  /* Success */
}

void safe_division_usage(void) {
    int quotient, remainder;  /* Will be initialized by function */

    if (safe_divide_with_remainder(17, 5, &quotient, &remainder) == 0) {
        printf("17 / 5 = %d remainder %d\n", quotient, remainder);
    } else {
        printf("Division failed\n");
    }

    /* Test error case */
    if (safe_divide_with_remainder(10, 0, &quotient, &remainder) != 0) {
        printf("Division by zero handled safely\n");
        printf("Safe defaults: quotient = %d, remainder = %d\n", quotient, remainder);
    }
}

/* COMPLIANT: Function with struct output parameter */
typedef struct {
    int min, max;
    double average;
    int count;
} ArrayStats;

int safe_calculate_array_stats(const int *array, int size, ArrayStats *stats) {
    /* Validate parameters */
    if (array == NULL || stats == NULL || size <= 0) {
        if (stats != NULL) {
            /* Initialize output structure even on error */
            stats->min = 0;
            stats->max = 0;
            stats->average = 0.0;
            stats->count = 0;
        }
        return -1;
    }

    /* Initialize stats structure */
    stats->min = array[0];
    stats->max = array[0];
    stats->count = size;

    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += array[i];
        if (array[i] < stats->min) stats->min = array[i];
        if (array[i] > stats->max) stats->max = array[i];
    }

    stats->average = (double)sum / size;
    return 0;
}

void safe_stats_usage(void) {
    int data[] = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    int data_size = sizeof(data) / sizeof(data[0]);
    ArrayStats stats;  /* Will be initialized by function */

    if (safe_calculate_array_stats(data, data_size, &stats) == 0) {
        printf("Array statistics:\n");
        printf("  Count: %d\n", stats.count);
        printf("  Min: %d\n", stats.min);
        printf("  Max: %d\n", stats.max);
        printf("  Average: %.2f\n", stats.average);
    } else {
        printf("Statistics calculation failed\n");
    }
}

/* COMPLIANT: Function pointer with safe initialization */
typedef int (*SafeMathOperation)(int a, int b);

int safe_add(int a, int b) { return a + b; }
int safe_multiply(int a, int b) { return a * b; }

void safe_function_pointer_usage(void) {
    /* Initialize function pointer explicitly */
    SafeMathOperation operation = safe_add;

    int result = operation(5, 3);
    printf("Addition result: %d\n", result);

    /* Change operation safely */
    operation = safe_multiply;
    result = operation(5, 3);
    printf("Multiplication result: %d\n", result);
}

/* COMPLIANT: Recursive function with proper initialization */
int safe_factorial(int n) {
    if (n < 0) {
        return -1;  /* Error case */
    }
    if (n <= 1) {
        return 1;   /* Base case */
    }
    return n * safe_factorial(n - 1);
}

void safe_recursive_usage(void) {
    for (int i = 0; i <= 5; i++) {
        int result = safe_factorial(i);
        printf("%d! = %d\n", i, result);
    }
}

int main(void) {
    printf("=== Safe Function Interfaces Demo ===\n");

    printf("1. Safe coordinate processing:\n");
    safe_coordinate_usage();

    printf("\n2. Safe calculation with all cases handled:\n");
    printf("5 + 3 = %d\n", safe_calculate_result(5, 3, 1));
    printf("5 / 0 = %d\n", safe_calculate_result(5, 0, 4));
    printf("Unknown op = %d\n", safe_calculate_result(5, 3, 99));

    printf("\n3. Safe callback usage:\n");
    safe_callback_usage();

    printf("\n4. Safe variadic function:\n");
    safe_variadic_usage();

    printf("\n5. Safe division with multiple outputs:\n");
    safe_division_usage();

    printf("\n6. Safe array statistics:\n");
    safe_stats_usage();

    printf("\n7. Safe function pointer usage:\n");
    safe_function_pointer_usage();

    printf("\n8. Safe recursive function:\n");
    safe_recursive_usage();

    return 0;
}