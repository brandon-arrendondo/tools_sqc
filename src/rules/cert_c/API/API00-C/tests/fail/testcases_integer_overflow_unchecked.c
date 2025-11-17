/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: integer_overflow_unchecked.c
 *
 * This case demonstrates violations where functions don't validate
 * integer parameters for potential overflow conditions.
 */

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

/* NON-COMPLIANT: No check for integer overflow in addition */
int add_integers(int a, int b) {
    /* Direct addition without overflow check */
    return a + b;  /* Could overflow if a + b > INT_MAX */
}

/* NON-COMPLIANT: No check for integer overflow in multiplication */
size_t calculate_buffer_size(size_t count, size_t element_size) {
    /* Direct multiplication without overflow check */
    return count * element_size;  /* Could overflow for large values */
}

/* NON-COMPLIANT: No check for integer underflow in subtraction */
unsigned int subtract_unsigned(unsigned int a, unsigned int b) {
    /* Direct subtraction without underflow check */
    return a - b;  /* Could underflow if b > a */
}

/* NON-COMPLIANT: No validation of shift amount */
int left_shift(int value, int shift_amount) {
    /* Shifting without validation */
    return value << shift_amount;  /* Undefined if shift_amount >= 32 or < 0 */
}

/* NON-COMPLIANT: No check for signed integer overflow in negation */
int negate_value(int value) {
    /* Direct negation without check */
    return -value;  /* INT_MIN negation causes overflow */
}

/* NON-COMPLIANT: No validation for array allocation size */
int *allocate_int_array(int num_elements) {
    /* Allocating without overflow check */
    return malloc(num_elements * sizeof(int));  /* Could overflow */
}

/* NON-COMPLIANT: No validation for power calculation */
long calculate_power(int base, int exponent) {
    long result = 1;
    /* No overflow checking during multiplication */
    for (int i = 0; i < exponent; i++) {
        result *= base;  /* Could overflow quickly */
    }
    return result;
}

/* NON-COMPLIANT: No validation for factorial calculation */
unsigned long factorial(unsigned int n) {
    unsigned long result = 1;
    /* No overflow checking */
    for (unsigned int i = 2; i <= n; i++) {
        result *= i;  /* Will overflow for n > 20 */
    }
    return result;
}

int main(void) {
    /* Examples of dangerous integer operations */
    // add_integers(INT_MAX, 1);  /* Integer overflow */
    // calculate_buffer_size(SIZE_MAX / 2, 3);  /* Size overflow */
    // subtract_unsigned(5, 10);  /* Unsigned underflow */
    // left_shift(1, 35);  /* Invalid shift amount */
    // negate_value(INT_MIN);  /* Overflow on negation */
    // allocate_int_array(INT_MAX);  /* Allocation size overflow */
    // calculate_power(10, 20);  /* Power overflow */
    // factorial(25);  /* Factorial overflow */

    printf("Integer functions compiled but lack overflow validation\n");
    return 0;
}