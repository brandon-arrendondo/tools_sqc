/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Pass Case: safe_multiple_functions.c
 *
 * This case demonstrates compliant usage when dealing with multiple
 * ENV30-C functions and their interaction.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <errno.h>

/* COMPLIANT: Safe handling of multiple function return values */
void safe_multiple_function_usage(void) {
    /* Get values from multiple functions immediately */
    const char *env_user = getenv("USER");
    const char *current_locale = setlocale(LC_ALL, NULL);

    time_t now = time(NULL);
    const char *time_str = ctime(&now);

    errno = ENOENT;
    const char *error_str = strerror(errno);

    /* Use all values immediately in formatted output */
    printf("System status report:\n");
    printf("  User: %s\n", env_user ?: "(not set)");
    printf("  Locale: %s\n", current_locale ?: "(unknown)");
    printf("  Time: %s", time_str ?: "(unknown)\n");
    printf("  Last error: %s\n", error_str ?: "(unknown)");
}

/* COMPLIANT: Safe sequential processing with copying */
void safe_sequential_processing(void) {
    /* Process environment variables safely */
    const char *home = getenv("HOME");
    const char *path = getenv("PATH");
    const char *shell = getenv("SHELL");

    /* Create copies for processing */
    char *home_copy = home ? strdup(home) : strdup("(unset)");
    char *path_copy = path ? strdup(path) : strdup("(unset)");
    char *shell_copy = shell ? strdup(shell) : strdup("(unset)");

    if (home_copy && path_copy && shell_copy) {
        /* Safe to process copies */
        printf("Environment analysis:\n");
        printf("  Home directory: %s (length: %zu)\n", home_copy, strlen(home_copy));
        printf("  PATH length: %zu\n", strlen(path_copy));
        printf("  Shell: %s\n", shell_copy);

        /* Count PATH components in copy */
        if (path && strlen(path_copy) > 0) {
            int path_count = 1;
            for (char *p = path_copy; *p; p++) {
                if (*p == ':') path_count++;
            }
            printf("  PATH has %d components\n", path_count);
        }
    }

    /* Cleanup */
    free(home_copy);
    free(path_copy);
    free(shell_copy);
}

/* COMPLIANT: Safe error handling with context */
void safe_error_handling_with_context(void) {
    const char *operation_file = "/etc/shadow";

    /* Attempt operation that will likely fail */
    FILE *file = fopen(operation_file, "r");

    if (file == NULL) {
        /* Get error information immediately */
        const char *error_msg = strerror(errno);
        const char *user = getenv("USER");

        time_t now = time(NULL);
        const char *time_str = ctime(&now);

        /* Create comprehensive error report */
        size_t report_size = 500;
        char *error_report = malloc(report_size);

        if (error_report != NULL) {
            snprintf(error_report, report_size,
                    "SECURITY ALERT: User '%s' attempted to access '%s' at %s"
                    "Error: %s",
                    user ?: "(unknown)",
                    operation_file,
                    time_str ?: "(unknown time)\n",
                    error_msg ?: "(unknown error)");

            printf("Security report:\n%s\n", error_report);
            free(error_report);
        }
    } else {
        fclose(file);
        printf("File access succeeded\n");
    }
}

/* COMPLIANT: Safe locale and time correlation */
void safe_locale_time_correlation(void) {
    /* Get locale information */
    struct lconv *lc = localeconv();
    const char *time_locale = setlocale(LC_TIME, NULL);

    time_t now = time(NULL);
    struct tm *time_info = localtime(&now);

    if (lc != NULL && time_info != NULL) {
        /* Create formatted output using safe methods */
        char time_buffer[100];
        char currency_info[100];

        /* Use strftime for safe time formatting */
        strftime(time_buffer, sizeof(time_buffer),
                "%A, %B %d, %Y at %H:%M:%S", time_info);

        /* Use locale info for currency formatting */
        snprintf(currency_info, sizeof(currency_info),
                "Currency: %s, Decimal: %s",
                lc->currency_symbol ?: "(none)",
                lc->decimal_point ?: ".");

        printf("Locale and time correlation:\n");
        printf("  Time locale: %s\n", time_locale ?: "(unknown)");
        printf("  Formatted time: %s\n", time_buffer);
        printf("  %s\n", currency_info);
    }
}

/* COMPLIANT: Safe configuration loading simulation */
void safe_configuration_loading(void) {
    /* Simulate loading configuration from environment */
    const char *config_vars[] = {
        "APP_NAME", "APP_VERSION", "LOG_LEVEL",
        "DATABASE_URL", "API_KEY", "DEBUG_MODE"
    };
    int num_vars = sizeof(config_vars) / sizeof(config_vars[0]);

    printf("Configuration loading:\n");

    for (int i = 0; i < num_vars; i++) {
        const char *value = getenv(config_vars[i]);

        if (value != NULL) {
            /* Create safe copy for processing */
            char *processed_value = malloc(strlen(value) + 50);

            if (processed_value != NULL) {
                /* Safe processing in copy */
                if (strstr(config_vars[i], "PASSWORD") ||
                    strstr(config_vars[i], "KEY") ||
                    strstr(config_vars[i], "SECRET")) {
                    strcpy(processed_value, "[REDACTED]");
                } else {
                    strcpy(processed_value, value);
                }

                printf("  %s = %s\n", config_vars[i], processed_value);
                free(processed_value);
            }
        } else {
            printf("  %s = (not set)\n", config_vars[i]);
        }
    }
}

/* COMPLIANT: Safe system information gathering */
void safe_system_info_gathering(void) {
    /* Gather system information safely */
    printf("System information:\n");

    /* Environment info */
    const char *system_vars[] = {"HOME", "USER", "HOSTNAME", "LANG", "TZ"};
    int num_system_vars = sizeof(system_vars) / sizeof(system_vars[0]);

    for (int i = 0; i < num_system_vars; i++) {
        const char *value = getenv(system_vars[i]);
        printf("  %s: %s\n", system_vars[i], value ?: "(not set)");
    }

    /* Time info */
    time_t now = time(NULL);
    printf("  Current time: %s", ctime(&now));

    /* Locale info */
    const char *locale = setlocale(LC_ALL, NULL);
    printf("  Locale: %s\n", locale ?: "(unknown)");

    /* Error state */
    errno = 0;  /* Clear errno */
    const char *clean_error = strerror(errno);
    printf("  System status: %s\n", clean_error);
}

int main(void) {
    printf("=== ENV30-C Safe Multiple Functions Usage Demo ===\n");

    /* Set up some test environment */
    setenv("USER", "testuser", 1);
    setenv("HOME", "/home/testuser", 1);
    setenv("PATH", "/usr/bin:/bin:/usr/local/bin", 1);
    setenv("SHELL", "/bin/bash", 1);
    setenv("APP_NAME", "TestApp", 1);
    setenv("APP_VERSION", "1.0.0", 1);
    setenv("LOG_LEVEL", "INFO", 1);
    setenv("API_KEY", "secret123456", 1);

    printf("\n1. Safe multiple function usage:\n");
    safe_multiple_function_usage();

    printf("\n2. Safe sequential processing:\n");
    safe_sequential_processing();

    printf("\n3. Safe error handling with context:\n");
    safe_error_handling_with_context();

    printf("\n4. Safe locale and time correlation:\n");
    safe_locale_time_correlation();

    printf("\n5. Safe configuration loading:\n");
    safe_configuration_loading();

    printf("\n6. Safe system information gathering:\n");
    safe_system_info_gathering();

    return 0;
}