/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Pass Case: safe_strerror_usage.c
 *
 * This case demonstrates compliant usage of strerror() by properly
 * handling return values without modification.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

/* COMPLIANT: Safe immediate use of strerror */
void safe_immediate_error_reporting(void) {
    /* Generate an error */
    int fd = open("/nonexistent/file", O_RDONLY);

    if (fd == -1) {
        /* Safe immediate use of strerror return value */
        printf("Error opening file: %s\n", strerror(errno));
    } else {
        close(fd);
    }
}

/* COMPLIANT: Safe error message copying */
void safe_error_message_copy(void) {
    /* Generate EACCES error */
    int fd = open("/etc/passwd", O_WRONLY);

    if (fd == -1) {
        const char *error_msg = strerror(errno);

        if (error_msg != NULL) {
            /* Create a copy for safe modification */
            char *error_copy = malloc(strlen(error_msg) + 20);

            if (error_copy != NULL) {
                sprintf(error_copy, "SYSTEM ERROR: %s", error_msg);
                printf("Enhanced error: %s\n", error_copy);
                free(error_copy);
            }
        }
    } else {
        close(fd);
    }
}

/* COMPLIANT: Safe error logging function */
void safe_log_error(const char *operation, int error_code) {
    const char *error_msg = strerror(error_code);

    /* Use error message immediately in formatted output */
    printf("[ERROR] %s failed: %s (errno: %d)\n",
           operation, error_msg ?: "Unknown error", error_code);
}

/* COMPLIANT: Safe error message with context */
void safe_error_with_context(void) {
    /* Generate ENOENT error */
    const char *filename = "/does/not/exist";
    int fd = open(filename, O_RDONLY);

    if (fd == -1) {
        const char *error_msg = strerror(errno);

        /* Build context message in new buffer */
        size_t context_size = strlen(filename) + strlen(error_msg) + 50;
        char *context_msg = malloc(context_size);

        if (context_msg != NULL) {
            snprintf(context_msg, context_size,
                    "Failed to open '%s': %s", filename, error_msg);
            printf("Contextual error: %s\n", context_msg);
            free(context_msg);
        }
    } else {
        close(fd);
    }
}

/* COMPLIANT: Safe error comparison */
void safe_error_comparison(void) {
    /* Generate specific errors for comparison */
    errno = ENOENT;
    const char *enoent_msg = strerror(errno);

    errno = EACCES;
    const char *eacces_msg = strerror(errno);

    /* Safe immediate comparison */
    printf("Error messages:\n");
    printf("  ENOENT: %s\n", enoent_msg);
    printf("  EACCES: %s\n", eacces_msg);

    if (strcmp(enoent_msg, eacces_msg) == 0) {
        printf("Messages are identical\n");
    } else {
        printf("Messages are different\n");
    }
}

/* COMPLIANT: Safe thread-safe alternative (where available) */
void safe_threadsafe_error_reporting(int error_code) {
    char error_buffer[256];

#ifdef _GNU_SOURCE
    /* GNU-specific strerror_r returns char* */
    char *result = strerror_r(error_code, error_buffer, sizeof(error_buffer));
    printf("Thread-safe error (GNU): %s\n", result);
#else
    /* POSIX strerror_r returns int */
    int result = strerror_r(error_code, error_buffer, sizeof(error_buffer));
    if (result == 0) {
        printf("Thread-safe error (POSIX): %s\n", error_buffer);
    } else {
        printf("strerror_r failed for error code %d\n", error_code);
    }
#endif
}

/* COMPLIANT: Safe error code validation */
void safe_error_validation(void) {
    /* Test various error codes */
    int test_errors[] = {ENOENT, EACCES, EINVAL, ENOMEM, EBUSY};
    int num_errors = sizeof(test_errors) / sizeof(test_errors[0]);

    printf("Error code validation:\n");
    for (int i = 0; i < num_errors; i++) {
        const char *msg = strerror(test_errors[i]);
        printf("  Error %d: %s\n", test_errors[i], msg ?: "Invalid error code");
    }
}

/* COMPLIANT: Safe error message formatting */
void safe_error_formatting(const char *function_name, int line_number) {
    /* Generate an error */
    int result = chmod("/nonexistent", 0644);

    if (result == -1) {
        const char *error_msg = strerror(errno);

        /* Create formatted error message in new buffer */
        char *formatted_error = malloc(strlen(function_name) +
                                      strlen(error_msg) + 100);

        if (formatted_error != NULL) {
            sprintf(formatted_error,
                   "Function '%s' at line %d failed: %s",
                   function_name, line_number, error_msg);
            printf("Formatted error: %s\n", formatted_error);
            free(formatted_error);
        }
    }
}

/* COMPLIANT: Safe multiple error handling */
void safe_multiple_error_handling(void) {
    /* Generate and handle multiple errors safely */
    int errors[] = {ENOENT, EACCES, EINVAL};
    const char *operations[] = {"file_open", "permission_check", "parameter_validation"};
    int num_ops = sizeof(errors) / sizeof(errors[0]);

    for (int i = 0; i < num_ops; i++) {
        errno = errors[i];
        safe_log_error(operations[i], errno);
    }
}

/* COMPLIANT: Safe error message length checking */
void safe_error_length_check(void) {
    errno = ENAMETOOLONG;
    const char *error_msg = strerror(errno);

    if (error_msg != NULL) {
        size_t msg_len = strlen(error_msg);
        printf("Error message length: %zu characters\n", msg_len);

        /* If we need to truncate for display, create a copy */
        if (msg_len > 50) {
            char *truncated = malloc(51);
            if (truncated != NULL) {
                strncpy(truncated, error_msg, 47);
                strcpy(truncated + 47, "...");
                printf("Truncated error: %s\n", truncated);
                free(truncated);
            }
        } else {
            printf("Full error message: %s\n", error_msg);
        }
    }
}

int main(void) {
    printf("=== ENV30-C Safe strerror() Usage Demo ===\n");

    printf("\n1. Safe immediate error reporting:\n");
    safe_immediate_error_reporting();

    printf("\n2. Safe error message copy:\n");
    safe_error_message_copy();

    printf("\n3. Safe error logging:\n");
    safe_log_error("test_operation", ENOENT);

    printf("\n4. Safe error with context:\n");
    safe_error_with_context();

    printf("\n5. Safe error comparison:\n");
    safe_error_comparison();

    printf("\n6. Safe thread-safe error reporting:\n");
    safe_threadsafe_error_reporting(EACCES);

    printf("\n7. Safe error validation:\n");
    safe_error_validation();

    printf("\n8. Safe error formatting:\n");
    safe_error_formatting("test_function", 123);

    printf("\n9. Safe multiple error handling:\n");
    safe_multiple_error_handling();

    printf("\n10. Safe error length check:\n");
    safe_error_length_check();

    return 0;
}