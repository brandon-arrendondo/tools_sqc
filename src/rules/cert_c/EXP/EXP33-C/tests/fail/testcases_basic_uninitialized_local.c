/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: basic_uninitialized_local.c
 *
 * This case demonstrates basic violations of EXP33-C by reading
 * uninitialized local variables before assignment.
 */

#include <stdio.h>
#include <stdlib.h>

/* NON-COMPLIANT: Reading uninitialized local variable */
int get_sign(int number) {
    int sign;  /* Uninitialized variable */

    if (number > 0) {
        sign = 1;
    } else if (number < 0) {
        sign = -1;
    }
    /* No initialization for number == 0 case */

    return sign;  /* Reading uninitialized memory when number == 0 */
}

/* NON-COMPLIANT: Multiple uninitialized variables */
void compute_stats(int *array, int size) {
    int sum, max, min;  /* All uninitialized */

    if (size > 0) {
        sum = 0;
        max = array[0];
        min = array[0];

        for (int i = 1; i < size; i++) {
            sum += array[i];
            if (array[i] > max) max = array[i];
            if (array[i] < min) min = array[i];
        }
    }

    /* Reading uninitialized variables when size <= 0 */
    printf("Sum: %d, Max: %d, Min: %d\n", sum, max, min);
}

/* NON-COMPLIANT: Uninitialized pointer usage */
void process_buffer(void) {
    char *buffer;  /* Uninitialized pointer */

    /* Some conditional logic that might not execute */
    if (rand() % 2) {
        buffer = malloc(100);
    }

    /* Reading uninitialized pointer */
    if (buffer != NULL) {  /* Undefined behavior - buffer is uninitialized */
        strcpy(buffer, "Hello");
        printf("%s\n", buffer);
        free(buffer);
    }
}

/* NON-COMPLIANT: Loop variable not initialized */
int find_element(int *array, int size, int target) {
    int i;  /* Uninitialized loop variable */

    /* Loop condition reads uninitialized i */
    while (i < size) {  /* Undefined behavior */
        if (array[i] == target) {
            return i;
        }
        i++;  /* i is still uninitialized on first iteration */
    }

    return -1;
}

/* NON-COMPLIANT: Function parameter passed uninitialized */
void unsafe_function_call(void) {
    int value;  /* Uninitialized */
    int result;

    /* Passing uninitialized value to function */
    result = abs(value);  /* abs() reads uninitialized memory */

    printf("Result: %d\n", result);
}

int main(void) {
    printf("=== Basic Uninitialized Local Variables Demo ===\n");

    /* Test 1: Uninitialized sign function */
    printf("Sign of 0: %d\n", get_sign(0));  /* Undefined behavior */

    /* Test 2: Empty array stats */
    int empty_array[1] = {0};
    compute_stats(empty_array, 0);  /* Undefined behavior */

    /* Test 3: Uninitialized pointer */
    process_buffer();  /* Undefined behavior */

    /* Test 4: Uninitialized loop */
    int test_array[] = {1, 2, 3, 4, 5};
    int index = find_element(test_array, 5, 3);  /* Undefined behavior */
    printf("Found at index: %d\n", index);

    /* Test 5: Uninitialized function parameter */
    unsafe_function_call();  /* Undefined behavior */

    return 0;
}