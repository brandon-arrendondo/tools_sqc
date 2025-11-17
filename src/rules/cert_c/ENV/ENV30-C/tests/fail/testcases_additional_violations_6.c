/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: additional_violations_6.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <errno.h>

/* NON-COMPLIANT: Mixed function violations */
void unsafe_mixed_modifications(void) {
    char *user = getenv("USER");
    char *locale = setlocale(LC_ALL, "C");
    errno = ENOENT;
    char *error = strerror(errno);

    /* VIOLATION: Multiple modifications */
    if (user) user[0] = 'X';  /* Undefined behavior */
    if (locale) locale[0] = 'Y';  /* Undefined behavior */
    if (error) error[0] = 'Z';  /* Undefined behavior */

    printf("Modified user: %s\n", user ?: "(null)");
    printf("Modified locale: %s\n", locale ?: "(null)");
    printf("Modified error: %s\n", error ?: "(null)");
}

/* NON-COMPLIANT: Complex string operations */
void unsafe_complex_operations(void) {
    char *value = getenv("COMPLEX_VAR");
    if (value) {
        /* VIOLATION: Complex in-place modifications */
        size_t len = strlen(value);
        for (size_t i = 0; i < len / 2; i++) {
            char temp = value[i];
            value[i] = value[len - 1 - i];  /* Undefined behavior */
            value[len - 1 - i] = temp;      /* Undefined behavior */
        }
        printf("Reversed: %s\n", value);
    }
}

int main(void) {
    setenv("USER", "testuser", 1);
    setenv("COMPLEX_VAR", "hello_world", 1);

    unsafe_mixed_modifications();
    unsafe_complex_operations();
    return 0;
}