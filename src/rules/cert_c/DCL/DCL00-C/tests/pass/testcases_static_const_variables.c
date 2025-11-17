/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Pass Case: static_const_variables.c
 *
 * This case demonstrates compliant code that properly uses static const
 * variables for function-local constants and file-scope constants.
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

/* COMPLIANT: File-scope static const variables */
static const int FILE_SCOPE_CONSTANT = 42;
static const double FILE_SCOPE_PI = 3.141592653589793;
static const char * const FILE_SCOPE_MESSAGE = "File scope constant message";

/* COMPLIANT: Static const array at file scope */
static const int FILE_SCOPE_PRIMES[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
static const size_t FILE_SCOPE_PRIMES_COUNT = sizeof(FILE_SCOPE_PRIMES) / sizeof(FILE_SCOPE_PRIMES[0]);

int calculate_area_circle(double radius) {
    /* COMPLIANT: Function-local static const */
    static const double PI = 3.141592653589793;
    static const char * const FUNCTION_NAME = "calculate_area_circle";

    printf("  Function: %s\\n", FUNCTION_NAME);
    printf("  Using static const PI = %.6f\\n", PI);

    double area = PI * radius * radius;
    printf("  Circle area (radius=%.2f): %.2f\\n", radius, area);

    return 0;
}

void demonstrate_counter_function(void) {
    /* COMPLIANT: Static const for function behavior configuration */
    static const int MAX_COUNT = 5;
    static const char * const COUNTER_FORMAT = "Counter: %d/%d\\n";

    /* Non-const static for state (this is appropriate) */
    static int counter = 0;

    printf("\\nDemo Counter Function:\\n");
    printf("  Max count is static const: %d\\n", MAX_COUNT);

    counter++;
    printf(COUNTER_FORMAT, counter, MAX_COUNT);

    if (counter >= MAX_COUNT) {
        printf("  Counter reached maximum!\\n");
        counter = 0;  /* Reset for next demo */
    }
}

void string_processing_function(const char *input) {
    /* COMPLIANT: Static const lookup tables within function */
    static const char VOWELS[] = "aeiouAEIOU";
    static const char * const PROCESSING_STAGES[] = {
        "Input validation",
        "Character analysis",
        "Result generation",
        "Output formatting"
    };
    static const size_t NUM_STAGES = sizeof(PROCESSING_STAGES) / sizeof(PROCESSING_STAGES[0]);

    printf("\\nString Processing Function:\\n");
    printf("  Input: '%s'\\n", input ? input : "(null)");

    if (!input) {
        printf("  Error: NULL input\\n");
        return;
    }

    /* Process through stages using static const data */
    for (size_t stage = 0; stage < NUM_STAGES; stage++) {
        printf("  Stage %zu: %s\\n", stage + 1, PROCESSING_STAGES[stage]);
    }

    /* Count vowels using static const lookup */
    int vowel_count = 0;
    for (size_t i = 0; input[i] != '\\0'; i++) {
        if (strchr(VOWELS, input[i]) != NULL) {
            vowel_count++;
        }
    }

    printf("  Analysis: %d vowels found\\n", vowel_count);
}

void demonstrate_configuration_reader(void) {
    /* COMPLIANT: Static const configuration within function */
    static const struct {
        const char *key;
        const char *default_value;
        const char *description;
    } CONFIG_DEFAULTS[] = {
        {"server_host", "localhost", "Default server hostname"},
        {"server_port", "8080", "Default server port"},
        {"max_connections", "100", "Maximum concurrent connections"},
        {"timeout_seconds", "30", "Connection timeout in seconds"},
        {"log_level", "INFO", "Default logging level"}
    };
    static const size_t CONFIG_COUNT = sizeof(CONFIG_DEFAULTS) / sizeof(CONFIG_DEFAULTS[0]);

    printf("\\nConfiguration Reader Function:\\n");
    printf("  Using static const configuration defaults:\\n");

    for (size_t i = 0; i < CONFIG_COUNT; i++) {
        printf("    %-16s = %-10s (%s)\\n",
               CONFIG_DEFAULTS[i].key,
               CONFIG_DEFAULTS[i].default_value,
               CONFIG_DEFAULTS[i].description);
    }
}

void demonstrate_state_machine(int state) {
    /* COMPLIANT: Static const state machine data */
    static const char * const STATE_NAMES[] = {
        "IDLE",
        "INITIALIZING",
        "RUNNING",
        "PAUSED",
        "STOPPING",
        "ERROR"
    };
    static const int NUM_STATES = sizeof(STATE_NAMES) / sizeof(STATE_NAMES[0]);

    static const struct {
        int from_state;
        int to_state;
        const char *action;
    } VALID_TRANSITIONS[] = {
        {0, 1, "start_initialization"},
        {1, 2, "initialization_complete"},
        {2, 3, "pause_request"},
        {3, 2, "resume_request"},
        {2, 4, "stop_request"},
        {4, 0, "stop_complete"},
        {-1, 5, "error_occurred"}  /* -1 means from any state */
    };
    static const size_t TRANSITION_COUNT = sizeof(VALID_TRANSITIONS) / sizeof(VALID_TRANSITIONS[0]);

    printf("\\nState Machine Function:\\n");

    if (state < 0 || state >= NUM_STATES) {
        printf("  Invalid state: %d\\n", state);
        return;
    }

    printf("  Current state: %s (%d)\\n", STATE_NAMES[state], state);

    printf("  Valid transitions from %s:\\n", STATE_NAMES[state]);
    for (size_t i = 0; i < TRANSITION_COUNT; i++) {
        if (VALID_TRANSITIONS[i].from_state == state ||
            VALID_TRANSITIONS[i].from_state == -1) {
            int to_state = VALID_TRANSITIONS[i].to_state;
            if (to_state >= 0 && to_state < NUM_STATES) {
                printf("    -> %s via %s\\n",
                       STATE_NAMES[to_state],
                       VALID_TRANSITIONS[i].action);
            }
        }
    }
}

void demonstrate_file_scope_usage(void) {
    printf("\\nFile Scope Static Const Usage:\\n");
    printf("  File scope constant: %d\\n", FILE_SCOPE_CONSTANT);
    printf("  File scope PI: %.6f\\n", FILE_SCOPE_PI);
    printf("  File scope message: %s\\n", FILE_SCOPE_MESSAGE);

    printf("  File scope primes (%zu numbers): ", FILE_SCOPE_PRIMES_COUNT);
    for (size_t i = 0; i < FILE_SCOPE_PRIMES_COUNT; i++) {
        printf("%d ", FILE_SCOPE_PRIMES[i]);
    }
    printf("\\n");

    /* Using file scope constants in calculations */
    double circle_area = FILE_SCOPE_PI * 5.0 * 5.0;
    printf("  Calculated area using file scope PI: %.2f\\n", circle_area);
}

int main(void) {
    /* COMPLIANT: Main function static const */
    static const char * const DEMO_TITLE = "Static Const Variables Demonstration";
    static const char * const SEPARATOR = "==========================================";

    printf("%s\\n", SEPARATOR);
    printf("%s\\n", DEMO_TITLE);
    printf("%s\\n", SEPARATOR);

    demonstrate_file_scope_usage();

    calculate_area_circle(3.0);
    calculate_area_circle(7.5);

    demonstrate_counter_function();
    demonstrate_counter_function();
    demonstrate_counter_function();

    string_processing_function("Hello World");
    string_processing_function("Programming");

    demonstrate_configuration_reader();

    demonstrate_state_machine(0);  /* IDLE */
    demonstrate_state_machine(2);  /* RUNNING */

    printf("\\n%s\\n", SEPARATOR);
    printf("Demo completed successfully\\n");
    printf("%s\\n", SEPARATOR);

    return 0;
}