/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: validation_violations.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* NON-COMPLIANT: Input validation with modification */
void unsafe_input_validation(void) {
    char *input = getenv("USER_INPUT");
    if (input) {
        /* VIOLATION: "Sanitizing" input by modifying in place */
        for (char *p = input; *p; p++) {
            if (!isalnum(*p)) {
                *p = '_';  /* Undefined behavior */
            }
        }
        printf("Sanitized input: %s\n", input);
    }
}

/* NON-COMPLIANT: Case normalization */
void unsafe_case_normalization(void) {
    char *value = getenv("OPTION");
    if (value) {
        /* VIOLATION: Converting to lowercase for comparison */
        for (char *p = value; *p; p++) {
            *p = tolower(*p);  /* Undefined behavior */
        }
        printf("Normalized option: %s\n", value);
    }
}

int main(void) {
    setenv("USER_INPUT", "hello@world!", 1);
    setenv("OPTION", "TRUE", 1);

    unsafe_input_validation();
    unsafe_case_normalization();
    return 0;
}