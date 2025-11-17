/*
 * Rule: API00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger API00-C violation
 */

/*
 * CERT C API00-C Pass Case: validated_math_operations.c
 *
 * This case demonstrates compliant mathematical functions with
 * comprehensive parameter validation and domain checking.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <float.h>
#include <fenv.h>

/* Mathematical operation result structure */
typedef struct {
    int success;
    double value;
    char error_message[128];
} MathResult;

/* COMPLIANT: Safe division with zero checking and overflow detection */
MathResult safe_divide(double dividend, double divisor) {
    MathResult result = {0, 0.0, ""};

    /* Check for division by zero */
    if (divisor == 0.0) {
        errno = EDOM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Division by zero");
        return result;
    }

    /* Check for potential overflow */
    if (dividend != 0.0) {
        double abs_dividend = fabs(dividend);
        double abs_divisor = fabs(divisor);

        if (abs_dividend > DBL_MAX * abs_divisor) {
            errno = ERANGE;
            snprintf(result.error_message, sizeof(result.error_message),
                    "Division would overflow");
            return result;
        }

        if (abs_divisor > abs_dividend / DBL_MIN) {
            errno = ERANGE;
            snprintf(result.error_message, sizeof(result.error_message),
                    "Division would underflow to zero");
            return result;
        }
    }

    /* Perform division */
    double quotient = dividend / divisor;

    /* Check for NaN or infinity */
    if (!isfinite(quotient)) {
        errno = ERANGE;
        snprintf(result.error_message, sizeof(result.error_message),
                "Division result is not finite");
        return result;
    }

    result.success = 1;
    result.value = quotient;
    snprintf(result.error_message, sizeof(result.error_message),
            "Division successful: %.6f / %.6f = %.6f", dividend, divisor, quotient);

    return result;
}

/* COMPLIANT: Safe square root with domain validation */
MathResult safe_sqrt(double value) {
    MathResult result = {0, 0.0, ""};

    /* Check domain: value must be non-negative */
    if (value < 0.0) {
        errno = EDOM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Square root of negative number: %.6f", value);
        return result;
    }

    /* Check for NaN input */
    if (isnan(value)) {
        errno = EDOM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Square root of NaN");
        return result;
    }

    /* Compute square root */
    double sqrt_value = sqrt(value);

    /* Verify result is finite */
    if (!isfinite(sqrt_value)) {
        errno = ERANGE;
        snprintf(result.error_message, sizeof(result.error_message),
                "Square root result is not finite");
        return result;
    }

    result.success = 1;
    result.value = sqrt_value;
    snprintf(result.error_message, sizeof(result.error_message),
            "Square root successful: sqrt(%.6f) = %.6f", value, sqrt_value);

    return result;
}

/* COMPLIANT: Safe logarithm with domain and base validation */
MathResult safe_log(double value, double base) {
    MathResult result = {0, 0.0, ""};

    /* Validate value domain */
    if (value <= 0.0) {
        errno = EDOM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Logarithm of non-positive number: %.6f", value);
        return result;
    }

    /* Validate base */
    if (base <= 0.0 || base == 1.0) {
        errno = EDOM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid logarithm base: %.6f", base);
        return result;
    }

    /* Check for NaN inputs */
    if (isnan(value) || isnan(base)) {
        errno = EDOM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Logarithm with NaN input");
        return result;
    }

    /* Compute logarithm using change of base formula */
    double log_value = log(value);
    double log_base = log(base);
    double result_value = log_value / log_base;

    /* Check result validity */
    if (!isfinite(result_value)) {
        errno = ERANGE;
        snprintf(result.error_message, sizeof(result.error_message),
                "Logarithm result is not finite");
        return result;
    }

    result.success = 1;
    result.value = result_value;
    snprintf(result.error_message, sizeof(result.error_message),
            "Logarithm successful: log_%.1f(%.6f) = %.6f", base, value, result_value);

    return result;
}

/* COMPLIANT: Safe power function with overflow checking */
MathResult safe_power(double base, double exponent) {
    MathResult result = {0, 0.0, ""};

    /* Check for NaN inputs */
    if (isnan(base) || isnan(exponent)) {
        errno = EDOM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Power function with NaN input");
        return result;
    }

    /* Special case: 0^0 is undefined */
    if (base == 0.0 && exponent == 0.0) {
        errno = EDOM;
        snprintf(result.error_message, sizeof(result.error_message),
                "0^0 is undefined");
        return result;
    }

    /* Check for negative base with non-integer exponent */
    if (base < 0.0 && floor(exponent) != exponent) {
        errno = EDOM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Negative base %.6f with non-integer exponent %.6f", base, exponent);
        return result;
    }

    /* Estimate result magnitude to prevent overflow */
    if (base != 0.0 && exponent != 0.0) {
        double log_abs_base = log(fabs(base));
        double estimated_log_result = exponent * log_abs_base;

        if (estimated_log_result > log(DBL_MAX)) {
            errno = ERANGE;
            snprintf(result.error_message, sizeof(result.error_message),
                    "Power operation would overflow");
            return result;
        }

        if (estimated_log_result < log(DBL_MIN)) {
            errno = ERANGE;
            snprintf(result.error_message, sizeof(result.error_message),
                    "Power operation would underflow");
            return result;
        }
    }

    /* Compute power */
    double power_result = pow(base, exponent);

    /* Check result validity */
    if (!isfinite(power_result)) {
        errno = ERANGE;
        snprintf(result.error_message, sizeof(result.error_message),
                "Power result is not finite");
        return result;
    }

    result.success = 1;
    result.value = power_result;
    snprintf(result.error_message, sizeof(result.error_message),
            "Power successful: %.6f^%.6f = %.6f", base, exponent, power_result);

    return result;
}

/* COMPLIANT: Safe trigonometric functions with domain checking */
MathResult safe_asin(double value) {
    MathResult result = {0, 0.0, ""};

    /* Check domain: value must be in [-1, 1] */
    if (value < -1.0 || value > 1.0) {
        errno = EDOM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Arcsine domain error: %.6f not in [-1, 1]", value);
        return result;
    }

    /* Check for NaN */
    if (isnan(value)) {
        errno = EDOM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Arcsine of NaN");
        return result;
    }

    /* Compute arcsine */
    double asin_value = asin(value);

    /* Result should always be finite for valid input */
    if (!isfinite(asin_value)) {
        errno = ERANGE;
        snprintf(result.error_message, sizeof(result.error_message),
                "Arcsine result is not finite");
        return result;
    }

    result.success = 1;
    result.value = asin_value;
    snprintf(result.error_message, sizeof(result.error_message),
            "Arcsine successful: asin(%.6f) = %.6f radians", value, asin_value);

    return result;
}

/* COMPLIANT: Safe array statistics with validation */
typedef struct {
    int success;
    double mean;
    double std_dev;
    double min_val;
    double max_val;
    char error_message[128];
} StatResult;

StatResult safe_calculate_statistics(const double *values, size_t count) {
    StatResult result = {0, 0.0, 0.0, 0.0, 0.0, ""};

    /* Validate parameters */
    if (!values) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Null values array");
        return result;
    }

    if (count == 0) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Empty array");
        return result;
    }

    /* Check for reasonable array size */
    const size_t MAX_ARRAY_SIZE = 1000000;  /* 1 million elements */
    if (count > MAX_ARRAY_SIZE) {
        errno = ERANGE;
        snprintf(result.error_message, sizeof(result.error_message),
                "Array too large: %zu elements", count);
        return result;
    }

    /* Validate all values are finite */
    for (size_t i = 0; i < count; i++) {
        if (!isfinite(values[i])) {
            errno = EDOM;
            snprintf(result.error_message, sizeof(result.error_message),
                    "Non-finite value at index %zu: %.6f", i, values[i]);
            return result;
        }
    }

    /* Calculate mean with overflow protection */
    double sum = 0.0;
    double min_val = values[0];
    double max_val = values[0];

    for (size_t i = 0; i < count; i++) {
        /* Check for overflow in sum */
        if ((values[i] > 0 && sum > DBL_MAX - values[i]) ||
            (values[i] < 0 && sum < -DBL_MAX - values[i])) {
            errno = ERANGE;
            snprintf(result.error_message, sizeof(result.error_message),
                    "Sum overflow at index %zu", i);
            return result;
        }

        sum += values[i];

        if (values[i] < min_val) min_val = values[i];
        if (values[i] > max_val) max_val = values[i];
    }

    double mean = sum / (double)count;

    /* Calculate standard deviation */
    double variance_sum = 0.0;
    for (size_t i = 0; i < count; i++) {
        double diff = values[i] - mean;
        double squared_diff = diff * diff;

        /* Check for overflow in variance sum */
        if (variance_sum > DBL_MAX - squared_diff) {
            errno = ERANGE;
            snprintf(result.error_message, sizeof(result.error_message),
                    "Variance calculation overflow");
            return result;
        }

        variance_sum += squared_diff;
    }

    double variance = variance_sum / (double)(count - 1);
    double std_dev = sqrt(variance);

    /* Verify all results are finite */
    if (!isfinite(mean) || !isfinite(std_dev) || !isfinite(min_val) || !isfinite(max_val)) {
        errno = ERANGE;
        snprintf(result.error_message, sizeof(result.error_message),
                "Non-finite result in statistics");
        return result;
    }

    result.success = 1;
    result.mean = mean;
    result.std_dev = std_dev;
    result.min_val = min_val;
    result.max_val = max_val;
    snprintf(result.error_message, sizeof(result.error_message),
            "Statistics calculated for %zu values", count);

    return result;
}

int main(void) {
    printf("=== Validated Math Operations Demo ===\n\n");

    /* Test safe division */
    printf("1. Safe division operations:\n");
    MathResult div1 = safe_divide(10.0, 3.0);
    if (div1.success) {
        printf("   %s\n", div1.error_message);
    } else {
        printf("   Error: %s\n", div1.error_message);
    }

    MathResult div2 = safe_divide(10.0, 0.0);  /* Should fail */
    if (!div2.success) {
        printf("   Correctly rejected: %s\n", div2.error_message);
    }

    /* Test safe square root */
    printf("\n2. Safe square root operations:\n");
    MathResult sqrt1 = safe_sqrt(16.0);
    if (sqrt1.success) {
        printf("   %s\n", sqrt1.error_message);
    }

    MathResult sqrt2 = safe_sqrt(-4.0);  /* Should fail */
    if (!sqrt2.success) {
        printf("   Correctly rejected: %s\n", sqrt2.error_message);
    }

    /* Test safe logarithm */
    printf("\n3. Safe logarithm operations:\n");
    MathResult log1 = safe_log(100.0, 10.0);
    if (log1.success) {
        printf("   %s\n", log1.error_message);
    }

    MathResult log2 = safe_log(-5.0, 10.0);  /* Should fail */
    if (!log2.success) {
        printf("   Correctly rejected: %s\n", log2.error_message);
    }

    MathResult log3 = safe_log(10.0, 1.0);  /* Should fail */
    if (!log3.success) {
        printf("   Correctly rejected: %s\n", log3.error_message);
    }

    /* Test safe power */
    printf("\n4. Safe power operations:\n");
    MathResult pow1 = safe_power(2.0, 8.0);
    if (pow1.success) {
        printf("   %s\n", pow1.error_message);
    }

    MathResult pow2 = safe_power(-2.0, 2.5);  /* Should fail */
    if (!pow2.success) {
        printf("   Correctly rejected: %s\n", pow2.error_message);
    }

    /* Test safe arcsine */
    printf("\n5. Safe arcsine operations:\n");
    MathResult asin1 = safe_asin(0.5);
    if (asin1.success) {
        printf("   %s\n", asin1.error_message);
    }

    MathResult asin2 = safe_asin(2.0);  /* Should fail */
    if (!asin2.success) {
        printf("   Correctly rejected: %s\n", asin2.error_message);
    }

    /* Test array statistics */
    printf("\n6. Safe array statistics:\n");
    double test_data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    size_t data_count = sizeof(test_data) / sizeof(test_data[0]);

    StatResult stats = safe_calculate_statistics(test_data, data_count);
    if (stats.success) {
        printf("   %s\n", stats.error_message);
        printf("   Mean: %.3f, Std Dev: %.3f\n", stats.mean, stats.std_dev);
        printf("   Range: [%.3f, %.3f]\n", stats.min_val, stats.max_val);
    } else {
        printf("   Error: %s\n", stats.error_message);
    }

    /* Test with NULL array */
    StatResult null_stats = safe_calculate_statistics(NULL, 10);
    if (!null_stats.success) {
        printf("   Correctly rejected NULL array: %s\n", null_stats.error_message);
    }

    /* Test with array containing NaN */
    double bad_data[] = {1.0, 2.0, NAN, 4.0, 5.0};
    StatResult bad_stats = safe_calculate_statistics(bad_data, 5);
    if (!bad_stats.success) {
        printf("   Correctly rejected array with NaN: %s\n", bad_stats.error_message);
    }

    printf("\n=== Math operations demo completed ===\n");
    return 0;
}