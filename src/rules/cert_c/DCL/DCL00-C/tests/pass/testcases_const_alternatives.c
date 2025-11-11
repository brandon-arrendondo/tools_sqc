/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Pass Case: const_alternatives.c
 *
 * This case demonstrates compliant alternatives to const qualification,
 * including enums, macros, and when const is not necessary.
 */

#include <stdio.h>

/* COMPLIANT: Using enum instead of const int for related constants */
enum ErrorCodes {
    ERROR_SUCCESS = 0,
    ERROR_INVALID_ARGUMENT = 1,
    ERROR_OUT_OF_MEMORY = 2,
    ERROR_FILE_NOT_FOUND = 3,
    ERROR_PERMISSION_DENIED = 4,
    ERROR_NETWORK_TIMEOUT = 5
};

enum LogLevel {
    LOG_TRACE = 0,
    LOG_DEBUG = 1,
    LOG_INFO = 2,
    LOG_WARN = 3,
    LOG_ERROR = 4,
    LOG_FATAL = 5
};

/* COMPLIANT: Using macros for compile-time constants */
#define MAX_BUFFER_SIZE 4096
#define MAX_USERNAME_LENGTH 32
#define MAX_PASSWORD_LENGTH 128
#define DEFAULT_PORT 8080
#define APPLICATION_NAME "Const Alternatives Demo"
#define VERSION_STRING "1.0.0"

/* COMPLIANT: Mathematical constants as macros */
#define PI 3.141592653589793
#define E 2.718281828459045
#define SQRT_2 1.414213562373095

/* COMPLIANT: Bit manipulation macros */
#define BIT(n) (1U << (n))
#define SET_BIT(reg, bit) ((reg) |= BIT(bit))
#define CLEAR_BIT(reg, bit) ((reg) &= ~BIT(bit))
#define TOGGLE_BIT(reg, bit) ((reg) ^= BIT(bit))
#define TEST_BIT(reg, bit) (((reg) & BIT(bit)) != 0)

void demonstrate_enum_constants(void) {
    printf("Enum Constants Demonstration:\\n");

    /* Using enum constants instead of const int */
    enum ErrorCodes current_error = ERROR_SUCCESS;
    enum LogLevel current_log_level = LOG_INFO;

    printf("  Error codes enum:\\n");
    printf("    SUCCESS: %d\\n", ERROR_SUCCESS);
    printf("    INVALID_ARGUMENT: %d\\n", ERROR_INVALID_ARGUMENT);
    printf("    OUT_OF_MEMORY: %d\\n", ERROR_OUT_OF_MEMORY);
    printf("    FILE_NOT_FOUND: %d\\n", ERROR_FILE_NOT_FOUND);
    printf("    PERMISSION_DENIED: %d\\n", ERROR_PERMISSION_DENIED);
    printf("    NETWORK_TIMEOUT: %d\\n", ERROR_NETWORK_TIMEOUT);

    printf("\\n  Log level enum:\\n");
    printf("    TRACE: %d\\n", LOG_TRACE);
    printf("    DEBUG: %d\\n", LOG_DEBUG);
    printf("    INFO: %d\\n", LOG_INFO);
    printf("    WARN: %d\\n", LOG_WARN);
    printf("    ERROR: %d\\n", LOG_ERROR);
    printf("    FATAL: %d\\n", LOG_FATAL);

    /* Using enums in switch statements */
    printf("\\n  Current status: ");
    switch (current_error) {
        case ERROR_SUCCESS:
            printf("Operation successful\\n");
            break;
        case ERROR_INVALID_ARGUMENT:
            printf("Invalid argument provided\\n");
            break;
        case ERROR_OUT_OF_MEMORY:
            printf("Out of memory\\n");
            break;
        default:
            printf("Unknown error\\n");
            break;
    }

    printf("  Current log level: %d\\n", current_log_level);
}

void demonstrate_macro_constants(void) {
    printf("\\nMacro Constants Demonstration:\\n");

    /* Using macros for configuration */
    printf("  Configuration macros:\\n");
    printf("    MAX_BUFFER_SIZE: %d\\n", MAX_BUFFER_SIZE);
    printf("    MAX_USERNAME_LENGTH: %d\\n", MAX_USERNAME_LENGTH);
    printf("    MAX_PASSWORD_LENGTH: %d\\n", MAX_PASSWORD_LENGTH);
    printf("    DEFAULT_PORT: %d\\n", DEFAULT_PORT);
    printf("    APPLICATION_NAME: %s\\n", APPLICATION_NAME);
    printf("    VERSION_STRING: %s\\n", VERSION_STRING);

    /* Using mathematical macros */
    printf("\\n  Mathematical macros:\\n");
    printf("    PI: %.15f\\n", PI);
    printf("    E: %.15f\\n", E);
    printf("    SQRT_2: %.15f\\n", SQRT_2);

    /* Calculations using macros */
    double radius = 5.0;
    double area = PI * radius * radius;
    double circumference = 2.0 * PI * radius;

    printf("\\n  Circle calculations (radius = %.1f):\\n", radius);
    printf("    Area: %.2f\\n", area);
    printf("    Circumference: %.2f\\n", circumference);

    /* Buffer allocation using macro */
    char buffer[MAX_BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "Buffer allocated with size %d", MAX_BUFFER_SIZE);
    printf("\\n  %s\\n", buffer);
}

void demonstrate_bit_manipulation_macros(void) {
    printf("\\nBit Manipulation Macros Demonstration:\\n");

    unsigned int register_value = 0;

    printf("  Initial register value: 0x%08X\\n", register_value);

    /* Using bit manipulation macros */
    SET_BIT(register_value, 0);
    printf("  After setting bit 0: 0x%08X\\n", register_value);

    SET_BIT(register_value, 3);
    printf("  After setting bit 3: 0x%08X\\n", register_value);

    SET_BIT(register_value, 7);
    printf("  After setting bit 7: 0x%08X\\n", register_value);

    printf("\\n  Bit testing:\\n");
    for (int bit = 0; bit < 8; bit++) {
        if (TEST_BIT(register_value, bit)) {
            printf("    Bit %d is SET\\n", bit);
        }
    }

    TOGGLE_BIT(register_value, 3);
    printf("\\n  After toggling bit 3: 0x%08X\\n", register_value);

    CLEAR_BIT(register_value, 0);
    printf("  After clearing bit 0: 0x%08X\\n", register_value);

    /* Show individual bit values */
    printf("\\n  Individual bit values:\\n");
    for (int i = 0; i < 8; i++) {
        printf("    BIT(%d) = 0x%08X\\n", i, BIT(i));
    }
}

/* COMPLIANT: Function parameters that don't need const */
void process_modifiable_data(int *values, size_t count) {
    printf("\\nProcessing Modifiable Data:\\n");
    printf("  Original values: ");

    for (size_t i = 0; i < count; i++) {
        printf("%d ", values[i]);
    }
    printf("\\n");

    /* Modify the values (this is the intended behavior) */
    for (size_t i = 0; i < count; i++) {
        values[i] *= 2;  /* Double each value */
    }

    printf("  Modified values: ");
    for (size_t i = 0; i < count; i++) {
        printf("%d ", values[i]);
    }
    printf("\\n");
}

/* COMPLIANT: Function using literal constants directly */
void demonstrate_literal_constants(void) {
    printf("\\nLiteral Constants Demonstration:\\n");

    /* Using literal constants directly when they appear only once */
    printf("  Circle area (radius=3): %.2f\\n", 3.14159 * 3.0 * 3.0);
    printf("  Temperature conversion: %.1f°C = %.1f°F\\n", 25.0, 25.0 * 9.0/5.0 + 32.0);

    /* Array with literal initialization */
    int fibonacci[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    size_t fib_count = sizeof(fibonacci) / sizeof(fibonacci[0]);

    printf("  Fibonacci sequence: ");
    for (size_t i = 0; i < fib_count; i++) {
        printf("%d ", fibonacci[i]);
    }
    printf("\\n");

    /* Direct string literals */
    printf("  Direct string usage: %s\\n", "This is a direct string literal");
    printf("  Formatted output: %s v%s\\n", "Application", "2.0");
}

/* COMPLIANT: Function showing when const is not needed */
void demonstrate_non_const_scenarios(void) {
    printf("\\nNon-Const Scenarios Demonstration:\\n");

    /* Loop variables that change */
    printf("  Countdown: ");
    for (int i = 10; i >= 1; i--) {
        printf("%d ", i);
    }
    printf("\\n");

    /* Accumulator variables */
    int sum = 0;
    int numbers[] = {1, 2, 3, 4, 5};
    size_t count = sizeof(numbers) / sizeof(numbers[0]);

    for (size_t i = 0; i < count; i++) {
        sum += numbers[i];  /* sum changes each iteration */
    }
    printf("  Sum of numbers: %d\\n", sum);

    /* State variables */
    int state = 0;  /* State can change */
    printf("  State progression: ");
    for (int step = 0; step < 5; step++) {
        printf("%d ", state);
        state = (state + 1) % 3;  /* Cycle through states 0, 1, 2 */
    }
    printf("\\n");
}

int main(void) {
    printf("=== %s v%s ===\\n\\n", APPLICATION_NAME, VERSION_STRING);

    demonstrate_enum_constants();
    demonstrate_macro_constants();
    demonstrate_bit_manipulation_macros();

    /* Demonstrate modifiable data processing */
    int test_data[] = {1, 2, 3, 4, 5};
    size_t data_count = sizeof(test_data) / sizeof(test_data[0]);
    process_modifiable_data(test_data, data_count);

    demonstrate_literal_constants();
    demonstrate_non_const_scenarios();

    printf("\\n=== Demonstration completed ===\\n");

    return ERROR_SUCCESS;  /* Using enum constant for return */
}