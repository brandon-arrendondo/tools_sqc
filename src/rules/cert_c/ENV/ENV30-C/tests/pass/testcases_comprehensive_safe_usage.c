/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Pass Case: comprehensive_safe_usage.c
 *
 * This case demonstrates a comprehensive collection of compliant
 * usage patterns for all ENV30-C covered functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <errno.h>

/* Utility function for safe string copying */
char *safe_string_copy(const char *source) {
    if (source == NULL) {
        return NULL;
    }

    size_t len = strlen(source);
    char *copy = malloc(len + 1);

    if (copy != NULL) {
        strcpy(copy, source);
    }

    return copy;
}

/* COMPLIANT: Comprehensive environment variable handling */
void comprehensive_env_handling(void) {
    printf("=== Comprehensive Environment Variable Handling ===\n");

    const char *env_vars[] = {
        "HOME", "PATH", "USER", "SHELL", "LANG",
        "TMPDIR", "EDITOR", "PAGER", "TERM", "DISPLAY"
    };
    int num_vars = sizeof(env_vars) / sizeof(env_vars[0]);

    for (int i = 0; i < num_vars; i++) {
        const char *value = getenv(env_vars[i]);

        if (value != NULL) {
            /* Safe immediate display */
            printf("  %s: %s\n", env_vars[i], value);

            /* If we need to process the value, create a copy */
            char *value_copy = safe_string_copy(value);
            if (value_copy != NULL) {
                printf("    Length: %zu characters\n", strlen(value_copy));

                /* Safe processing of copy */
                if (strlen(value_copy) > 50) {
                    printf("    Type: Long value\n");
                } else {
                    printf("    Type: Short value\n");
                }

                free(value_copy);
            }
        } else {
            printf("  %s: (not set)\n", env_vars[i]);
        }
    }
}

/* COMPLIANT: Comprehensive locale handling */
void comprehensive_locale_handling(void) {
    printf("\n=== Comprehensive Locale Handling ===\n");

    /* Save original locale */
    const char *original_locale = setlocale(LC_ALL, NULL);
    char *saved_locale = safe_string_copy(original_locale);

    printf("Original locale: %s\n", original_locale ?: "(null)");

    /* Test different locale categories */
    int categories[] = {LC_ALL, LC_CTYPE, LC_NUMERIC, LC_TIME, LC_COLLATE, LC_MONETARY, LC_MESSAGES};
    const char *category_names[] = {"LC_ALL", "LC_CTYPE", "LC_NUMERIC", "LC_TIME", "LC_COLLATE", "LC_MONETARY", "LC_MESSAGES"};
    int num_categories = sizeof(categories) / sizeof(categories[0]);

    for (int i = 1; i < num_categories; i++) {  /* Skip LC_ALL */
        const char *cat_locale = setlocale(categories[i], NULL);
        printf("  %s: %s\n", category_names[i], cat_locale ?: "(null)");
    }

    /* Get and display locale convention information */
    struct lconv *lc = localeconv();
    if (lc != NULL) {
        printf("Locale conventions:\n");
        printf("  Decimal point: '%s'\n", lc->decimal_point ?: "");
        printf("  Thousands separator: '%s'\n", lc->thousands_sep ?: "");
        printf("  Currency symbol: '%s'\n", lc->currency_symbol ?: "");
        printf("  International currency: '%s'\n", lc->int_curr_symbol ?: "");
        printf("  Positive sign: '%s'\n", lc->positive_sign ?: "");
        printf("  Negative sign: '%s'\n", lc->negative_sign ?: "");
    }

    /* Restore original locale */
    if (saved_locale != NULL) {
        setlocale(LC_ALL, saved_locale);
        printf("Restored locale to: %s\n", saved_locale);
        free(saved_locale);
    }
}

/* COMPLIANT: Comprehensive time handling */
void comprehensive_time_handling(void) {
    printf("\n=== Comprehensive Time Handling ===\n");

    time_t current_time = time(NULL);

    /* Safe immediate use of time functions */
    printf("Current time (ctime): %s", ctime(&current_time));

    struct tm *local_tm = localtime(&current_time);
    if (local_tm != NULL) {
        printf("Current time (asctime): %s", asctime(local_tm));

        /* Safe custom formatting using strftime */
        char time_buffer[100];

        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", local_tm);
        printf("ISO format: %s\n", time_buffer);

        strftime(time_buffer, sizeof(time_buffer), "%A, %B %d, %Y", local_tm);
        printf("Readable format: %s\n", time_buffer);

        strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S %Z", local_tm);
        printf("Time with zone: %s\n", time_buffer);
    }

    /* UTC time handling */
    struct tm *utc_tm = gmtime(&current_time);
    if (utc_tm != NULL) {
        char utc_buffer[100];
        strftime(utc_buffer, sizeof(utc_buffer), "%Y-%m-%d %H:%M:%S UTC", utc_tm);
        printf("UTC time: %s\n", utc_buffer);
    }
}

/* COMPLIANT: Comprehensive error handling */
void comprehensive_error_handling(void) {
    printf("\n=== Comprehensive Error Handling ===\n");

    /* Test various error codes */
    int test_errors[] = {
        ENOENT, EACCES, EINVAL, ENOMEM, EBUSY,
        EEXIST, EISDIR, ENOTDIR, EPERM, EAGAIN
    };
    int num_errors = sizeof(test_errors) / sizeof(test_errors[0]);

    printf("Error code descriptions:\n");
    for (int i = 0; i < num_errors; i++) {
        const char *error_desc = strerror(test_errors[i]);
        printf("  %d: %s\n", test_errors[i], error_desc ?: "Unknown error");
    }

    /* Demonstrate safe error message usage in context */
    printf("\nContextual error reporting:\n");
    for (int i = 0; i < 3; i++) {
        errno = test_errors[i];
        const char *error_msg = strerror(errno);

        /* Create contextual error message */
        char *context_msg = malloc(200);
        if (context_msg != NULL) {
            snprintf(context_msg, 200, "Operation failed with error %d: %s",
                    errno, error_msg);
            printf("  %s\n", context_msg);
            free(context_msg);
        }
    }
}

/* COMPLIANT: Safe application configuration example */
void safe_application_config(void) {
    printf("\n=== Safe Application Configuration ===\n");

    /* Configuration with defaults */
    typedef struct {
        char *app_name;
        char *version;
        char *log_level;
        char *data_dir;
        char *temp_dir;
        char *user_name;
    } AppConfig;

    AppConfig config = {0};

    /* Load configuration from environment with safe defaults */
    const char *app_name = getenv("APP_NAME");
    config.app_name = safe_string_copy(app_name ?: "MyApplication");

    const char *version = getenv("APP_VERSION");
    config.version = safe_string_copy(version ?: "1.0.0");

    const char *log_level = getenv("LOG_LEVEL");
    config.log_level = safe_string_copy(log_level ?: "INFO");

    const char *data_dir = getenv("DATA_DIR");
    if (data_dir == NULL) {
        const char *home = getenv("HOME");
        if (home != NULL) {
            size_t path_len = strlen(home) + 20;
            config.data_dir = malloc(path_len);
            if (config.data_dir != NULL) {
                snprintf(config.data_dir, path_len, "%s/.myapp", home);
            }
        } else {
            config.data_dir = safe_string_copy("/tmp/myapp");
        }
    } else {
        config.data_dir = safe_string_copy(data_dir);
    }

    const char *temp_dir = getenv("TMPDIR");
    config.temp_dir = safe_string_copy(temp_dir ?: "/tmp");

    const char *user_name = getenv("USER");
    config.user_name = safe_string_copy(user_name ?: "unknown");

    /* Display configuration */
    printf("Application Configuration:\n");
    printf("  Name: %s\n", config.app_name ?: "N/A");
    printf("  Version: %s\n", config.version ?: "N/A");
    printf("  Log Level: %s\n", config.log_level ?: "N/A");
    printf("  Data Directory: %s\n", config.data_dir ?: "N/A");
    printf("  Temp Directory: %s\n", config.temp_dir ?: "N/A");
    printf("  User: %s\n", config.user_name ?: "N/A");

    /* Cleanup */
    free(config.app_name);
    free(config.version);
    free(config.log_level);
    free(config.data_dir);
    free(config.temp_dir);
    free(config.user_name);
}

/* COMPLIANT: Safe logging system example */
void safe_logging_system(void) {
    printf("\n=== Safe Logging System ===\n");

    /* Get current time for log timestamp */
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[64];

    if (tm_info != NULL) {
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    } else {
        strcpy(timestamp, "UNKNOWN_TIME");
    }

    /* Get user and application info */
    const char *user = getenv("USER");
    const char *app_name = getenv("APP_NAME");

    /* Create log entries safely */
    const char *log_levels[] = {"INFO", "WARN", "ERROR"};
    const char *messages[] = {
        "Application started successfully",
        "Configuration file not found, using defaults",
        "Failed to connect to database"
    };

    for (int i = 0; i < 3; i++) {
        char *log_entry = malloc(500);
        if (log_entry != NULL) {
            snprintf(log_entry, 500, "[%s] %s - %s - User: %s, App: %s",
                    timestamp,
                    log_levels[i],
                    messages[i],
                    user ?: "unknown",
                    app_name ?: "unknown");

            printf("LOG: %s\n", log_entry);
            free(log_entry);
        }
    }
}

int main(void) {
    printf("=== ENV30-C Comprehensive Safe Usage Demo ===\n");

    /* Set up test environment */
    setenv("APP_NAME", "ENV30C_Demo", 1);
    setenv("APP_VERSION", "2.0.0", 1);
    setenv("LOG_LEVEL", "DEBUG", 1);

    comprehensive_env_handling();
    comprehensive_locale_handling();
    comprehensive_time_handling();
    comprehensive_error_handling();
    safe_application_config();
    safe_logging_system();

    printf("\n=== Comprehensive demo completed successfully ===\n");
    return 0;
}