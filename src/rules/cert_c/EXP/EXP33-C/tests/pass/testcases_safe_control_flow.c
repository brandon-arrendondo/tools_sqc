/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Pass Case: safe_control_flow.c
 *
 * This case demonstrates compliant control flow patterns that
 * ensure variables are properly initialized in all execution paths.
 */

#include <stdio.h>
#include <stdlib.h>

/* COMPLIANT: Safe loop with proper initialization */
void safe_for_loop_operations(void) {
    int sum = 0;        /* Initialize accumulator */
    int product = 1;    /* Initialize accumulator */

    for (int i = 1; i <= 5; i++) {  /* Loop variable initialized in declaration */
        sum += i;
        product *= i;
        printf("i=%d, sum=%d, product=%d\n", i, sum, product);
    }

    printf("Final sum: %d, Final product: %d\n", sum, product);
}

/* COMPLIANT: Safe while loop with proper initialization */
void safe_while_loop_operations(void) {
    int counter = 0;    /* Initialize loop variable */
    int total = 0;      /* Initialize accumulator */

    while (counter < 5) {
        total += counter;
        printf("counter=%d, total=%d\n", counter, total);
        counter++;      /* Safe increment of initialized variable */
    }

    printf("Final total: %d\n", total);
}

/* COMPLIANT: Safe do-while loop with proper initialization */
void safe_do_while_operations(void) {
    int value = 1;          /* Initialize control variable */
    int accumulator = 0;    /* Initialize accumulator */

    do {
        accumulator += value;   /* Safe operation on initialized variables */
        printf("value=%d, accumulator=%d\n", value, accumulator);
        value++;
    } while (value <= 5);

    printf("Final accumulator: %d\n", accumulator);
}

/* COMPLIANT: Safe conditional initialization covering all paths */
void safe_conditional_processing(int score) {
    char grade = 'F';       /* Default initialization covers all cases */
    const char *message = "No message";  /* Default initialization */

    if (score >= 90) {
        grade = 'A';
        message = "Excellent work!";
    } else if (score >= 80) {
        grade = 'B';
        message = "Good job!";
    } else if (score >= 70) {
        grade = 'C';
        message = "Satisfactory";
    } else if (score >= 60) {
        grade = 'D';
        message = "Needs improvement";
    } else {
        /* grade and message keep their default values */
        message = "Please study more";
    }

    printf("Score %d: Grade %c - %s\n", score, grade, message);
}

/* COMPLIANT: Safe switch statement with all cases handled */
int safe_switch_operations(int option) {
    int result = 0;  /* Default initialization */

    switch (option) {
        case 1:
            result = 10;
            break;
        case 2:
            result = 20;
            break;
        case 3:
            result = 30;
            break;
        case 4:
            result = 40;
            break;
        default:
            result = -1;  /* Explicit handling of unknown cases */
            break;
    }

    return result;  /* Always returns initialized value */
}

/* COMPLIANT: Safe nested loops with proper initialization */
void safe_nested_loops(void) {
    int total_sum = 0;  /* Initialize outer accumulator */

    for (int i = 1; i <= 3; i++) {
        int row_sum = 0;    /* Initialize inner accumulator for each iteration */

        for (int j = 1; j <= 3; j++) {
            int value = i * j;
            row_sum += value;
            printf("i=%d, j=%d, value=%d, row_sum=%d\n", i, j, value, row_sum);
        }

        total_sum += row_sum;
        printf("Row %d sum: %d, Total so far: %d\n", i, row_sum, total_sum);
    }

    printf("Final total sum: %d\n", total_sum);
}

/* COMPLIANT: Safe goto usage with proper initialization */
void safe_goto_usage(int condition) {
    int result = 0;     /* Initialize before any goto */
    const char *status = "unknown";  /* Initialize before any goto */

    if (condition > 10) {
        result = condition * 2;
        status = "doubled";
        goto success;
    } else if (condition < 0) {
        result = 0;
        status = "negative input";
        goto error_handling;
    } else {
        result = condition;
        status = "unchanged";
        goto success;
    }

success:
    printf("Success: result=%d, status=%s\n", result, status);
    return;

error_handling:
    printf("Error handled: result=%d, status=%s\n", result, status);
    return;
}

/* COMPLIANT: Safe exception-like error handling with initialization */
typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_INPUT,
    ERROR_CALCULATION,
    ERROR_MEMORY
} ErrorCode;

ErrorCode safe_complex_calculation(int input, int *output) {
    if (output == NULL) {
        return ERROR_INVALID_INPUT;
    }

    *output = 0;  /* Initialize output immediately */

    if (input < 0) {
        return ERROR_INVALID_INPUT;
    }

    if (input > 1000) {
        return ERROR_CALCULATION;
    }

    /* Perform calculation */
    *output = input * input + 10;
    return SUCCESS;
}

void safe_error_handling_usage(void) {
    int result;  /* Will be initialized by function */
    ErrorCode error;

    /* Test various inputs */
    int test_values[] = {5, -1, 1500, 25};
    int test_count = sizeof(test_values) / sizeof(test_values[0]);

    for (int i = 0; i < test_count; i++) {
        error = safe_complex_calculation(test_values[i], &result);

        switch (error) {
            case SUCCESS:
                printf("Input %d: Success, result = %d\n", test_values[i], result);
                break;
            case ERROR_INVALID_INPUT:
                printf("Input %d: Invalid input error, result = %d\n", test_values[i], result);
                break;
            case ERROR_CALCULATION:
                printf("Input %d: Calculation error, result = %d\n", test_values[i], result);
                break;
            case ERROR_MEMORY:
                printf("Input %d: Memory error, result = %d\n", test_values[i], result);
                break;
            default:
                printf("Input %d: Unknown error, result = %d\n", test_values[i], result);
                break;
        }
    }
}

/* COMPLIANT: Safe early return with initialization */
int safe_early_return_function(int *array, int size, int target) {
    if (array == NULL || size <= 0) {
        return -1;  /* Early return with defined value */
    }

    for (int i = 0; i < size; i++) {
        if (array[i] == target) {
            return i;   /* Return initialized value */
        }
    }

    return -1;  /* Not found */
}

void safe_early_return_usage(void) {
    int data[] = {10, 20, 30, 40, 50};
    int size = sizeof(data) / sizeof(data[0]);

    int index = safe_early_return_function(data, size, 30);
    if (index >= 0) {
        printf("Found 30 at index %d\n", index);
    } else {
        printf("30 not found\n");
    }

    index = safe_early_return_function(data, size, 99);
    if (index >= 0) {
        printf("Found 99 at index %d\n", index);
    } else {
        printf("99 not found\n");
    }

    /* Test edge cases */
    index = safe_early_return_function(NULL, 5, 10);
    printf("NULL array test: %d\n", index);

    index = safe_early_return_function(data, 0, 10);
    printf("Zero size test: %d\n", index);
}

int main(void) {
    printf("=== Safe Control Flow Demo ===\n");

    printf("1. Safe for loop:\n");
    safe_for_loop_operations();

    printf("\n2. Safe while loop:\n");
    safe_while_loop_operations();

    printf("\n3. Safe do-while loop:\n");
    safe_do_while_operations();

    printf("\n4. Safe conditional processing:\n");
    safe_conditional_processing(85);
    safe_conditional_processing(50);
    safe_conditional_processing(95);

    printf("\n5. Safe switch operations:\n");
    for (int i = 1; i <= 5; i++) {
        printf("Option %d: %d\n", i, safe_switch_operations(i));
    }

    printf("\n6. Safe nested loops:\n");
    safe_nested_loops();

    printf("\n7. Safe goto usage:\n");
    safe_goto_usage(15);
    safe_goto_usage(-5);
    safe_goto_usage(5);

    printf("\n8. Safe error handling:\n");
    safe_error_handling_usage();

    printf("\n9. Safe early return:\n");
    safe_early_return_usage();

    return 0;
}