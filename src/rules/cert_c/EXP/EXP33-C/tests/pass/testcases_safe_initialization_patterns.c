/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Pass Case: safe_initialization_patterns.c
 *
 * This case demonstrates compliant memory initialization patterns
 * that prevent reading uninitialized memory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* COMPLIANT: Immediate initialization at declaration */
int safe_get_sign(int number) {
    int sign = 0;  /* Initialized immediately */

    if (number > 0) {
        sign = 1;
    } else if (number < 0) {
        sign = -1;
    }
    /* All code paths now handled - sign has valid value */

    return sign;
}

/* COMPLIANT: Complete initialization before use */
void safe_compute_stats(int *array, int size) {
    int sum = 0;      /* Initialized */
    int max = 0;      /* Initialized to safe default */
    int min = 0;      /* Initialized to safe default */

    if (size > 0) {
        sum = 0;      /* Explicit initialization */
        max = array[0];
        min = array[0];

        for (int i = 1; i < size; i++) {
            sum += array[i];
            if (array[i] > max) max = array[i];
            if (array[i] < min) min = array[i];
        }
    }

    printf("Sum: %d, Max: %d, Min: %d\n", sum, max, min);
}

/* COMPLIANT: Safe pointer initialization */
void safe_process_buffer(void) {
    char *buffer = NULL;  /* Initialize to NULL */

    /* Deterministic allocation */
    buffer = malloc(100);
    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        return;
    }

    /* Initialize allocated memory */
    memset(buffer, 0, 100);
    strcpy(buffer, "Hello");

    printf("%s\n", buffer);
    free(buffer);
}

/* COMPLIANT: Loop variable properly initialized */
int safe_find_element(int *array, int size, int target) {
    if (array == NULL || size <= 0) {
        return -1;
    }

    for (int i = 0; i < size; i++) {  /* i initialized in for loop */
        if (array[i] == target) {
            return i;
        }
    }

    return -1;
}

/* COMPLIANT: Function parameters validated and defaults provided */
int safe_calculate_with_defaults(int a, int b, int operation) {
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

    return result;
}

/* COMPLIANT: Array initialization with designated initializers */
void safe_array_operations(void) {
    int numbers[10] = {0};  /* Initialize all elements to 0 */

    /* Partial explicit initialization with remainder zeroed */
    int values[5] = {1, 2, 3};  /* values[3] and values[4] are 0 */

    /* Print all elements safely */
    printf("Numbers: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    printf("Values: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", values[i]);
    }
    printf("\n");
}

/* COMPLIANT: String buffer safe initialization */
void safe_string_operations(void) {
    char buffer[256] = {0};  /* Initialize all bytes to 0 */

    /* Safe string operations */
    strncpy(buffer, "Hello", sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';  /* Ensure null termination */

    printf("Buffer content: %s\n", buffer);
    printf("Buffer length: %zu\n", strlen(buffer));
}

/* COMPLIANT: Conditional initialization with all paths covered */
void safe_conditional_processing(int score) {
    char grade = 'F';  /* Default initialization */

    if (score >= 90) {
        grade = 'A';
    } else if (score >= 80) {
        grade = 'B';
    } else if (score >= 70) {
        grade = 'C';
    } else if (score >= 60) {
        grade = 'D';
    }
    /* Else case covered by default initialization */

    printf("Grade for score %d: %c\n", score, grade);
}

int main(void) {
    printf("=== Safe Initialization Patterns Demo ===\n");

    /* Test 1: Safe sign function */
    printf("1. Safe sign function:\n");
    printf("Sign of 0: %d\n", safe_get_sign(0));
    printf("Sign of 5: %d\n", safe_get_sign(5));
    printf("Sign of -3: %d\n", safe_get_sign(-3));

    /* Test 2: Safe stats computation */
    printf("\n2. Safe stats computation:\n");
    int data[] = {1, 5, 3, 9, 2};
    safe_compute_stats(data, 5);
    safe_compute_stats(NULL, 0);  /* Edge case */

    /* Test 3: Safe buffer processing */
    printf("\n3. Safe buffer processing:\n");
    safe_process_buffer();

    /* Test 4: Safe element finding */
    printf("\n4. Safe element finding:\n");
    int index = safe_find_element(data, 5, 3);
    printf("Element 3 found at index: %d\n", index);

    /* Test 5: Safe calculations */
    printf("\n5. Safe calculations:\n");
    printf("5 + 3 = %d\n", safe_calculate_with_defaults(5, 3, 1));
    printf("5 / 0 = %d\n", safe_calculate_with_defaults(5, 0, 4));
    printf("Unknown op = %d\n", safe_calculate_with_defaults(5, 3, 99));

    /* Test 6: Safe array operations */
    printf("\n6. Safe array operations:\n");
    safe_array_operations();

    /* Test 7: Safe string operations */
    printf("\n7. Safe string operations:\n");
    safe_string_operations();

    /* Test 8: Safe conditional processing */
    printf("\n8. Safe conditional processing:\n");
    safe_conditional_processing(85);
    safe_conditional_processing(50);

    return 0;
}