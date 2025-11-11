/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: division_by_zero.c
 *
 * This case demonstrates violations where mathematical functions
 * don't validate parameters that could cause division by zero.
 */

#include <stdio.h>
#include <math.h>

/* NON-COMPLIANT: No check for zero divisor */
double divide_numbers(double numerator, double denominator) {
    /* Direct division without checking for zero */
    return numerator / denominator;  /* denominator could be zero */
}

/* NON-COMPLIANT: No validation of array for zero elements */
double calculate_harmonic_mean(double *values, size_t count) {
    double sum_reciprocals = 0.0;
    for (size_t i = 0; i < count; i++) {
        /* No check if values[i] is zero */
        sum_reciprocals += 1.0 / values[i];  /* Division by zero possible */
    }
    return count / sum_reciprocals;
}

/* NON-COMPLIANT: No validation for modulo by zero */
int modulo_operation(int dividend, int divisor) {
    /* Direct modulo without zero check */
    return dividend % divisor;  /* divisor could be zero */
}

/* NON-COMPLIANT: No validation for average calculation */
double calculate_average(int sum, int count) {
    /* Division without checking count */
    return (double)sum / count;  /* count could be zero */
}

/* NON-COMPLIANT: No validation for slope calculation */
double calculate_slope(double x1, double y1, double x2, double y2) {
    /* No check if x coordinates are the same */
    return (y2 - y1) / (x2 - x1);  /* Division by zero if x1 == x2 */
}

/* NON-COMPLIANT: No validation for percentage calculation */
double calculate_percentage(int part, int whole) {
    /* Direct calculation without zero check */
    return (part * 100.0) / whole;  /* whole could be zero */
}

/* NON-COMPLIANT: No validation for rate calculation */
double calculate_rate(double distance, double time) {
    /* No check for zero time */
    return distance / time;  /* time could be zero */
}

/* NON-COMPLIANT: No validation for reciprocal */
double get_reciprocal(double value) {
    /* Direct reciprocal without zero check */
    return 1.0 / value;  /* value could be zero */
}

int main(void) {
    double zero_array[] = {1.0, 2.0, 0.0, 4.0};

    /* Examples of dangerous operations */
    // divide_numbers(10.0, 0.0);  /* Division by zero */
    // calculate_harmonic_mean(zero_array, 4);  /* Has zero element */
    // modulo_operation(10, 0);  /* Modulo by zero */
    // calculate_average(100, 0);  /* Zero count */
    // calculate_slope(1.0, 2.0, 1.0, 5.0);  /* Vertical line */
    // calculate_percentage(50, 0);  /* Zero whole */
    // calculate_rate(100.0, 0.0);  /* Zero time */
    // get_reciprocal(0.0);  /* Reciprocal of zero */

    printf("Mathematical functions compiled but lack division by zero checks\n");
    return 0;
}