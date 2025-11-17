/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: complex_control_flow.c
 *
 * This case demonstrates violations in complex control flow scenarios
 * where uninitialized variables may be read.
 */

#include <stdio.h>
#include <stdlib.h>

/* NON-COMPLIANT: Complex nested loops with uninitialized variables */
void complex_nested_loops(void) {
    int sum, product, max_value;  /* All uninitialized */

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int value = i * j;

            if (i == 0 && j == 0) {
                sum = 0;
                product = 1;
                max_value = value;
            } else {
                sum += value;         /* sum uninitialized on first iteration when i*j != 0 */
                product *= value;     /* product uninitialized similarly */
                if (value > max_value) {  /* max_value uninitialized similarly */
                    max_value = value;
                }
            }
        }
    }

    printf("Sum: %d, Product: %d, Max: %d\n", sum, product, max_value);
}

/* NON-COMPLIANT: Goto statements with uninitialized variables */
void goto_flow_issues(int condition) {
    int result;  /* Uninitialized */

    if (condition > 10) {
        goto calculate;
    } else if (condition < 0) {
        goto error;
    }

    /* Normal path doesn't initialize result */
    goto end;

calculate:
    result = condition * 2;
    goto end;

error:
    printf("Error occurred\n");
    goto end;  /* result remains uninitialized */

end:
    printf("Result: %d\n", result);  /* May read uninitialized value */
}

int main(void) {
    printf("=== Complex Control Flow Demo ===\n");

    printf("1. Complex nested loops:\n");
    complex_nested_loops();

    printf("\n2. Goto flow issues:\n");
    goto_flow_issues(5);  /* Normal path - result uninitialized */

    return 0;
}