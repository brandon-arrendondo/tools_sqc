/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: multiple_function_violations.c
 *
 * This case demonstrates violations involving multiple ENV30-C functions
 * and the interaction between static buffer reuse.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <errno.h>

/* NON-COMPLIANT: Storing multiple return values and modifying them */
void unsafe_multiple_storage_and_modification(void) {
    /* Store return values from multiple functions */
    char *env_path = getenv("PATH");
    char *env_home = getenv("HOME");
    char *locale_str = setlocale(LC_ALL, NULL);

    /* VIOLATION: Modifying stored return values */
    if (env_path != NULL) {
        env_path[0] = 'X';  /* Undefined behavior */
    }

    if (env_home != NULL) {
        strcat(env_home, "/modified");  /* Undefined behavior */
    }

    if (locale_str != NULL) {
        locale_str[0] = 'L';  /* Undefined behavior */
    }

    printf("Modified PATH: %s\n", env_path ?: "(null)");
    printf("Modified HOME: %s\n", env_home ?: "(null)");
    printf("Modified locale: %s\n", locale_str ?: "(null)");
}

/* NON-COMPLIANT: Buffer reuse demonstration with modification */
void unsafe_buffer_reuse_with_modification(void) {
    /* First call to getenv */
    char *first_env = getenv("USER");
    printf("First env: %s\n", first_env ?: "(null)");

    /* Modify the first result */
    if (first_env != NULL) {
        /* VIOLATION: Modifying first result */
        first_env[0] = 'M';  /* Undefined behavior */
    }

    /* Second call to getenv (may reuse the same static buffer) */
    char *second_env = getenv("SHELL");
    printf("Second env: %s\n", second_env ?: "(null)");

    /* VIOLATION: first_env may now point to invalid or changed data */
    printf("First env after second call: %s\n", first_env ?: "(null)");

    /* VIOLATION: Modifying second result */
    if (second_env != NULL) {
        strcat(second_env, "_MODIFIED");  /* Undefined behavior */
    }
}

/* NON-COMPLIANT: Mixing time and error functions with modification */
void unsafe_time_error_mix(void) {
    time_t current_time = time(NULL);

    /* Get time string */
    char *time_str = ctime(&current_time);

    /* Generate error */
    errno = ENOENT;
    char *error_str = strerror(errno);

    /* VIOLATION: Modifying time string */
    if (time_str != NULL) {
        time_str[0] = 'T';  /* Undefined behavior */
    }

    /* VIOLATION: Modifying error string */
    if (error_str != NULL) {
        error_str[0] = 'E';  /* Undefined behavior */
    }

    printf("Modified time: %s", time_str ?: "(null)\n");
    printf("Modified error: %s\n", error_str ?: "(null)");
}

/* NON-COMPLIANT: Cross-thread unsafe usage with modification */
void unsafe_cross_function_modification(void) {
    /* Get locale information */
    struct lconv *lc = localeconv();
    char *locale_name = setlocale(LC_NUMERIC, NULL);

    /* VIOLATION: Modifying locale name */
    if (locale_name != NULL) {
        locale_name[0] = 'X';  /* Undefined behavior */
    }

    /* VIOLATION: Modifying locale structure */
    if (lc != NULL && lc->decimal_point != NULL) {
        lc->decimal_point[0] = '@';  /* Undefined behavior */
    }

    printf("Modified locale name: %s\n", locale_name ?: "(null)");
    printf("Modified decimal point: %s\n",
           (lc && lc->decimal_point) ? lc->decimal_point : "(null)");
}

int main(void) {
    printf("=== ENV30-C Multiple Function Violations ===\n");

    /* Set up some environment variables */
    setenv("PATH", "/usr/bin:/bin", 1);
    setenv("HOME", "/home/user", 1);
    setenv("USER", "testuser", 1);
    setenv("SHELL", "/bin/bash", 1);

    printf("\n1. Unsafe multiple storage and modification:\n");
    unsafe_multiple_storage_and_modification();

    printf("\n2. Unsafe buffer reuse with modification:\n");
    unsafe_buffer_reuse_with_modification();

    printf("\n3. Unsafe time/error mix:\n");
    unsafe_time_error_mix();

    printf("\n4. Unsafe cross-function modification:\n");
    unsafe_cross_function_modification();

    return 0;
}