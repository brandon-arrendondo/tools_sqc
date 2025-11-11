/*
 * Rule: API00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger API00-C violation
 */

/*
 * CERT C API00-C Pass Case: error_handling_patterns.c
 *
 * This case demonstrates compliant error handling patterns with
 * proper parameter validation and state preservation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>

/* Error handling result structure */
typedef struct {
    int error_code;
    char error_message[256];
    const char *function_name;
    const char *file_name;
    int line_number;
} ErrorInfo;

/* Resource management structure */
typedef struct {
    FILE *file_handle;
    void *memory_block;
    int socket_fd;
    int is_initialized;
} Resource;

/* Global error state (in practice, use thread-local storage) */
static ErrorInfo last_error = {0};
static jmp_buf error_recovery_point;
static int error_recovery_enabled = 0;

/* COMPLIANT: Safe error information recording */
void safe_set_error(int error_code, const char *message, const char *function, const char *file, int line) {
    /* Validate parameters - be defensive even in error handling */
    if (!message || !function || !file) {
        /* Fallback error information */
        last_error.error_code = (error_code != 0) ? error_code : EINVAL;
        snprintf(last_error.error_message, sizeof(last_error.error_message),
                "Error in error handling: NULL parameter");
        last_error.function_name = "unknown";
        last_error.file_name = "unknown";
        last_error.line_number = 0;
        return;
    }

    /* Validate message length */
    size_t message_len = strlen(message);
    if (message_len >= sizeof(last_error.error_message)) {
        last_error.error_code = error_code;
        snprintf(last_error.error_message, sizeof(last_error.error_message),
                "Error message too long (%zu chars)", message_len);
        last_error.function_name = function;
        last_error.file_name = file;
        last_error.line_number = line;
        return;
    }

    /* Record error information */
    last_error.error_code = error_code;
    strncpy(last_error.error_message, message, sizeof(last_error.error_message) - 1);
    last_error.error_message[sizeof(last_error.error_message) - 1] = '\0';
    last_error.function_name = function;
    last_error.file_name = file;
    last_error.line_number = line;
}

/* COMPLIANT: Safe error information retrieval */
ErrorInfo safe_get_last_error(void) {
    return last_error;
}

/* COMPLIANT: Safe error clearing */
void safe_clear_error(void) {
    memset(&last_error, 0, sizeof(last_error));
}

/* Convenient macro for error setting */
#define SET_ERROR(code, msg) safe_set_error((code), (msg), __func__, __FILE__, __LINE__)

/* COMPLIANT: Safe resource initialization with validation */
int safe_resource_init(Resource *resource) {
    /* Validate parameter */
    if (!resource) {
        SET_ERROR(EINVAL, "NULL resource pointer");
        return -1;
    }

    /* Initialize all fields to safe defaults */
    resource->file_handle = NULL;
    resource->memory_block = NULL;
    resource->socket_fd = -1;
    resource->is_initialized = 0;

    /* Attempt memory allocation */
    const size_t BUFFER_SIZE = 1024;
    resource->memory_block = malloc(BUFFER_SIZE);
    if (!resource->memory_block) {
        SET_ERROR(ENOMEM, "Failed to allocate resource buffer");
        return -1;
    }

    /* Initialize memory */
    memset(resource->memory_block, 0, BUFFER_SIZE);

    /* Mark as initialized only after all operations succeed */
    resource->is_initialized = 1;
    return 0;
}

/* COMPLIANT: Safe resource cleanup with validation */
void safe_resource_cleanup(Resource *resource) {
    /* Handle NULL resource gracefully */
    if (!resource) {
        return;  /* Safe to call on NULL */
    }

    /* Clean up in reverse order of initialization */
    if (resource->file_handle) {
        fclose(resource->file_handle);
        resource->file_handle = NULL;
    }

    if (resource->socket_fd >= 0) {
        close(resource->socket_fd);
        resource->socket_fd = -1;
    }

    if (resource->memory_block) {
        /* Clear memory before freeing for security */
        memset(resource->memory_block, 0, 1024);
        free(resource->memory_block);
        resource->memory_block = NULL;
    }

    resource->is_initialized = 0;
}

/* COMPLIANT: Safe file operation with rollback on error */
int safe_write_config_file(const char *filename, const char *config_data) {
    /* Validate parameters */
    if (!filename || !config_data) {
        SET_ERROR(EINVAL, "NULL filename or config data");
        return -1;
    }

    if (strlen(filename) == 0) {
        SET_ERROR(EINVAL, "Empty filename");
        return -1;
    }

    size_t data_len = strlen(config_data);
    if (data_len == 0) {
        SET_ERROR(EINVAL, "Empty config data");
        return -1;
    }

    /* Check for reasonable data size */
    const size_t MAX_CONFIG_SIZE = 1024 * 1024;  /* 1 MB */
    if (data_len > MAX_CONFIG_SIZE) {
        SET_ERROR(ERANGE, "Config data too large");
        return -1;
    }

    /* Create temporary filename for atomic write */
    const size_t temp_filename_size = strlen(filename) + 16;
    char *temp_filename = malloc(temp_filename_size);
    if (!temp_filename) {
        SET_ERROR(ENOMEM, "Cannot allocate temporary filename");
        return -1;
    }

    snprintf(temp_filename, temp_filename_size, "%s.tmp.%d", filename, getpid());

    /* Open temporary file */
    FILE *temp_file = fopen(temp_filename, "w");
    if (!temp_file) {
        free(temp_filename);
        SET_ERROR(errno, "Cannot create temporary file");
        return -1;
    }

    /* Write data to temporary file */
    size_t written = fwrite(config_data, 1, data_len, temp_file);
    int write_error = ferror(temp_file);
    fclose(temp_file);

    if (write_error || written != data_len) {
        /* Cleanup on write failure */
        unlink(temp_filename);
        free(temp_filename);
        SET_ERROR(EIO, "Write to temporary file failed");
        return -1;
    }

    /* Atomic rename to final filename */
    if (rename(temp_filename, filename) != 0) {
        unlink(temp_filename);
        free(temp_filename);
        SET_ERROR(errno, "Cannot rename temporary file");
        return -1;
    }

    free(temp_filename);
    safe_clear_error();  /* Success - clear any previous errors */
    return 0;
}

/* COMPLIANT: Safe operation with exception-style error handling */
int safe_complex_operation(int *input_array, size_t array_size, int **result_array, size_t *result_size) {
    /* Set up error recovery point */
    if (setjmp(error_recovery_point) != 0) {
        /* Error recovery - cleanup and return */
        if (result_array && *result_array) {
            free(*result_array);
            *result_array = NULL;
        }
        if (result_size) {
            *result_size = 0;
        }
        return -1;
    }

    error_recovery_enabled = 1;

    /* Validate parameters */
    if (!input_array || !result_array || !result_size) {
        SET_ERROR(EINVAL, "NULL parameter in complex operation");
        longjmp(error_recovery_point, 1);
    }

    if (array_size == 0) {
        SET_ERROR(EINVAL, "Empty input array");
        longjmp(error_recovery_point, 1);
    }

    if (array_size > 1000000) {  /* Reasonable limit */
        SET_ERROR(ERANGE, "Input array too large");
        longjmp(error_recovery_point, 1);
    }

    /* Initialize outputs */
    *result_array = NULL;
    *result_size = 0;

    /* Validate all input values are reasonable */
    for (size_t i = 0; i < array_size; i++) {
        if (input_array[i] < INT_MIN / 2 || input_array[i] > INT_MAX / 2) {
            SET_ERROR(ERANGE, "Input value out of safe range");
            longjmp(error_recovery_point, 1);
        }
    }

    /* Allocate result array */
    int *result = malloc(array_size * sizeof(int));
    if (!result) {
        SET_ERROR(ENOMEM, "Cannot allocate result array");
        longjmp(error_recovery_point, 1);
    }

    /* Process data (example: double each value) */
    for (size_t i = 0; i < array_size; i++) {
        /* Check for overflow */
        if (input_array[i] > INT_MAX / 2 || input_array[i] < INT_MIN / 2) {
            free(result);
            SET_ERROR(ERANGE, "Overflow in processing");
            longjmp(error_recovery_point, 1);
        }

        result[i] = input_array[i] * 2;
    }

    /* Success - commit results */
    *result_array = result;
    *result_size = array_size;
    error_recovery_enabled = 0;
    safe_clear_error();

    return 0;
}

/* COMPLIANT: Safe batch operation with partial success handling */
typedef struct {
    int processed_count;
    int success_count;
    int error_count;
    char summary[256];
} BatchResult;

BatchResult safe_batch_process(const char **items, size_t item_count, int (*processor)(const char *)) {
    BatchResult result = {0, 0, 0, ""};

    /* Validate parameters */
    if (!items || !processor) {
        snprintf(result.summary, sizeof(result.summary),
                "Invalid parameters: items=%p, processor=%p", (void*)items, (void*)processor);
        return result;
    }

    if (item_count == 0) {
        snprintf(result.summary, sizeof(result.summary), "No items to process");
        return result;
    }

    /* Validate reasonable batch size */
    const size_t MAX_BATCH_SIZE = 10000;
    if (item_count > MAX_BATCH_SIZE) {
        snprintf(result.summary, sizeof(result.summary),
                "Batch size %zu exceeds maximum %zu", item_count, MAX_BATCH_SIZE);
        return result;
    }

    /* Process each item, continuing on individual failures */
    for (size_t i = 0; i < item_count; i++) {
        result.processed_count++;

        /* Validate individual item */
        if (!items[i]) {
            result.error_count++;
            continue;  /* Skip NULL items */
        }

        /* Process item */
        safe_clear_error();  /* Clear previous errors */
        int process_result = processor(items[i]);

        if (process_result == 0) {
            result.success_count++;
        } else {
            result.error_count++;
        }
    }

    /* Generate summary */
    snprintf(result.summary, sizeof(result.summary),
            "Processed %d items: %d succeeded, %d failed",
            result.processed_count, result.success_count, result.error_count);

    return result;
}

/* Example processor function for batch testing */
int example_string_processor(const char *str) {
    if (!str || strlen(str) == 0) {
        SET_ERROR(EINVAL, "Empty string");
        return -1;
    }

    if (strlen(str) > 100) {
        SET_ERROR(ERANGE, "String too long");
        return -1;
    }

    /* Mock processing - fail if string starts with 'X' */
    if (str[0] == 'X' || str[0] == 'x') {
        SET_ERROR(ENOTSUP, "Strings starting with X not supported");
        return -1;
    }

    return 0;  /* Success */
}

int main(void) {
    printf("=== Error Handling Patterns Demo ===\n\n");

    /* Test basic error handling */
    printf("1. Basic error handling:\n");
    Resource test_resource;

    if (safe_resource_init(&test_resource) == 0) {
        printf("   Resource initialized successfully\n");
        safe_resource_cleanup(&test_resource);
        printf("   Resource cleaned up successfully\n");
    } else {
        ErrorInfo error = safe_get_last_error();
        printf("   Resource init failed: %s (%s:%d)\n",
               error.error_message, error.function_name, error.line_number);
    }

    /* Test NULL parameter handling */
    printf("\n2. NULL parameter handling:\n");
    if (safe_resource_init(NULL) != 0) {
        ErrorInfo error = safe_get_last_error();
        printf("   Correctly rejected NULL: %s\n", error.error_message);
    }

    /* Test file operation with rollback */
    printf("\n3. File operation with rollback:\n");
    const char *config_data = "server_port=8080\ndebug=true\nmax_clients=100\n";
    if (safe_write_config_file("test_config.txt", config_data) == 0) {
        printf("   Config file written successfully\n");
        unlink("test_config.txt");  /* Cleanup */
    } else {
        ErrorInfo error = safe_get_last_error();
        printf("   Config write failed: %s\n", error.error_message);
    }

    /* Test with invalid parameters */
    if (safe_write_config_file(NULL, config_data) != 0) {
        ErrorInfo error = safe_get_last_error();
        printf("   Correctly rejected NULL filename: %s\n", error.error_message);
    }

    /* Test complex operation with exception-style handling */
    printf("\n4. Complex operation with error recovery:\n");
    int input_data[] = {1, 2, 3, 4, 5};
    int *result_data = NULL;
    size_t result_size = 0;

    if (safe_complex_operation(input_data, 5, &result_data, &result_size) == 0) {
        printf("   Complex operation succeeded, processed %zu items\n", result_size);
        printf("   Results: ");
        for (size_t i = 0; i < result_size; i++) {
            printf("%d ", result_data[i]);
        }
        printf("\n");
        free(result_data);
    } else {
        ErrorInfo error = safe_get_last_error();
        printf("   Complex operation failed: %s\n", error.error_message);
    }

    /* Test with invalid input */
    if (safe_complex_operation(NULL, 5, &result_data, &result_size) != 0) {
        ErrorInfo error = safe_get_last_error();
        printf("   Correctly rejected NULL input: %s\n", error.error_message);
    }

    /* Test batch processing */
    printf("\n5. Batch processing:\n");
    const char *test_items[] = {
        "item1",
        "item2",
        "xbad_item",  /* Will fail */
        "item4",
        NULL,         /* Will fail */
        "item6"
    };
    size_t test_count = sizeof(test_items) / sizeof(test_items[0]);

    BatchResult batch_result = safe_batch_process(test_items, test_count, example_string_processor);
    printf("   %s\n", batch_result.summary);

    /* Test batch with NULL parameters */
    BatchResult null_batch = safe_batch_process(NULL, 5, example_string_processor);
    printf("   Null batch test: %s\n", null_batch.summary);

    printf("\n=== Error handling patterns demo completed ===\n");
    return 0;
}