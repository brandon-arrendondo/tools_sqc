/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Pass Case: safe_error_recovery.c
 *
 * This case demonstrates compliant error recovery patterns
 * using ENV30-C functions safely.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

/* COMPLIANT: Safe error recovery with logging */
int safe_file_operation_with_recovery(const char *filename) {
    int fd = -1;
    int retry_count = 0;
    const int max_retries = 3;

    while (retry_count < max_retries) {
        fd = open(filename, O_RDONLY);

        if (fd >= 0) {
            printf("Successfully opened %s on attempt %d\n", filename, retry_count + 1);
            return fd;
        }

        /* Safe error handling without modifying strerror result */
        const char *error_msg = strerror(errno);
        char *error_log = malloc(strlen(filename) + strlen(error_msg) + 100);

        if (error_log != NULL) {
            sprintf(error_log, "Attempt %d: Failed to open '%s': %s",
                   retry_count + 1, filename, error_msg);
            printf("ERROR: %s\n", error_log);
            free(error_log);
        }

        retry_count++;

        if (retry_count < max_retries) {
            printf("Retrying in 1 second...\n");
            sleep(1);
        }
    }

    printf("Failed to open %s after %d attempts\n", filename, max_retries);
    return -1;
}

/* COMPLIANT: Safe configuration fallback mechanism */
void safe_config_fallback(void) {
    const char *config_sources[] = {
        "PRIMARY_CONFIG",
        "SECONDARY_CONFIG",
        "FALLBACK_CONFIG"
    };
    int num_sources = sizeof(config_sources) / sizeof(config_sources[0]);

    printf("Configuration fallback mechanism:\n");

    for (int i = 0; i < num_sources; i++) {
        const char *config_path = getenv(config_sources[i]);

        if (config_path != NULL) {
            /* Test if configuration is accessible */
            int fd = open(config_path, O_RDONLY);

            if (fd >= 0) {
                close(fd);
                printf("  Using %s: %s\n", config_sources[i], config_path);
                return;
            } else {
                /* Safe error reporting */
                const char *error_msg = strerror(errno);
                printf("  %s (%s) failed: %s\n",
                       config_sources[i], config_path, error_msg);
            }
        } else {
            printf("  %s: not set\n", config_sources[i]);
        }
    }

    printf("  Using built-in defaults\n");
}

/* COMPLIANT: Safe environment validation with defaults */
void safe_environment_validation(void) {
    typedef struct {
        const char *var_name;
        const char *default_value;
        int required;
    } EnvVar;

    EnvVar env_vars[] = {
        {"HOME", "/tmp", 1},
        {"USER", "unknown", 1},
        {"TMPDIR", "/tmp", 0},
        {"EDITOR", "vi", 0},
        {"PAGER", "more", 0}
    };
    int num_vars = sizeof(env_vars) / sizeof(env_vars[0]);

    printf("Environment validation:\n");

    for (int i = 0; i < num_vars; i++) {
        const char *value = getenv(env_vars[i].var_name);

        if (value != NULL && strlen(value) > 0) {
            printf("  %s: %s (from environment)\n",
                   env_vars[i].var_name, value);
        } else {
            if (env_vars[i].required) {
                printf("  %s: %s (using default - required variable not set)\n",
                       env_vars[i].var_name, env_vars[i].default_value);
            } else {
                printf("  %s: %s (using default - optional)\n",
                       env_vars[i].var_name, env_vars[i].default_value);
            }
        }
    }
}

/* COMPLIANT: Safe resource cleanup with error handling */
void safe_resource_cleanup_demo(void) {
    const char *temp_file = "/tmp/test_file_env30c";

    printf("Resource cleanup demo:\n");

    /* Create a temporary file */
    int fd = open(temp_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if (fd >= 0) {
        printf("  Created temporary file: %s\n", temp_file);

        /* Write some data */
        const char *data = "Test data for ENV30-C demo";
        ssize_t written = write(fd, data, strlen(data));

        if (written > 0) {
            printf("  Wrote %zd bytes to file\n", written);
        } else {
            const char *error_msg = strerror(errno);
            printf("  Write failed: %s\n", error_msg);
        }

        close(fd);
        printf("  Closed file descriptor\n");

        /* Clean up */
        if (unlink(temp_file) == 0) {
            printf("  Successfully removed temporary file\n");
        } else {
            const char *error_msg = strerror(errno);
            printf("  Failed to remove file: %s\n", error_msg);
        }
    } else {
        const char *error_msg = strerror(errno);
        printf("  Failed to create file: %s\n", error_msg);
    }
}

int main(void) {
    printf("=== ENV30-C Safe Error Recovery Demo ===\n");

    /* Set up test environment */
    setenv("SECONDARY_CONFIG", "/etc/hosts", 1);  /* File that should exist */
    setenv("FALLBACK_CONFIG", "/dev/null", 1);   /* File that definitely exists */

    printf("\n1. Safe file operation with recovery:\n");
    int fd = safe_file_operation_with_recovery("/nonexistent/file");
    if (fd >= 0) {
        close(fd);
    }

    printf("\n2. Safe configuration fallback:\n");
    safe_config_fallback();

    printf("\n3. Safe environment validation:\n");
    safe_environment_validation();

    printf("\n4. Safe resource cleanup:\n");
    safe_resource_cleanup_demo();

    return 0;
}