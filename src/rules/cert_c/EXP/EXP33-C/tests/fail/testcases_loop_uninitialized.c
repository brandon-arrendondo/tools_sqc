/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: loop_uninitialized.c
 *
 * This case demonstrates violations involving uninitialized variables
 * in various loop constructs.
 */

#include <stdio.h>
#include <stdlib.h>

/* NON-COMPLIANT: For loop with uninitialized iterator */
void unsafe_for_loop(void) {
    int i;  /* Uninitialized */
    int sum = 0;

    for (i = 0; i < 10; i++) {  /* First comparison reads uninitialized i */
        sum += i;
    }

    printf("Sum: %d\n", sum);
}

/* NON-COMPLIANT: While loop with uninitialized condition */
void unsafe_while_loop(void) {
    int counter;  /* Uninitialized */
    int total = 0;

    while (counter < 5) {  /* Reading uninitialized counter */
        total += counter;
        counter++;
    }

    printf("Total: %d\n", total);
}

/* NON-COMPLIANT: Do-while loop with uninitialized accumulator */
void unsafe_do_while_loop(void) {
    int value = 1;
    int accumulator;  /* Uninitialized */

    do {
        accumulator += value;  /* Reading uninitialized accumulator */
        value++;
    } while (value <= 5);

    printf("Accumulator: %d\n", accumulator);
}

int main(void) {
    printf("=== Loop Uninitialized Demo ===\n");

    printf("1. Unsafe for loop:\n");
    unsafe_for_loop();

    printf("\n2. Unsafe while loop:\n");
    unsafe_while_loop();

    printf("\n3. Unsafe do-while loop:\n");
    unsafe_do_while_loop();

    return 0;
}