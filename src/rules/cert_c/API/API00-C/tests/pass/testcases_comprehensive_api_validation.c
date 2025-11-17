/*
 * Rule: API00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger API00-C violation
 */

/*
 * CERT C API00-C Pass Case: comprehensive_api_validation.c
 *
 * This case demonstrates a comprehensive API implementation with
 * complete parameter validation, error handling, and state management.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <limits.h>

/* API result codes */
typedef enum {
    API_SUCCESS = 0,
    API_ERROR_INVALID_PARAM = 1,
    API_ERROR_OUT_OF_RANGE = 2,
    API_ERROR_OUT_OF_MEMORY = 3,
    API_ERROR_INVALID_STATE = 4,
    API_ERROR_OPERATION_FAILED = 5
} ApiResult;

/* API context structure */
typedef struct {
    int is_initialized;
    char *session_id;
    time_t creation_time;
    size_t operation_count;
    char last_error[256];
    void **allocated_resources;
    size_t resource_count;
    size_t max_resources;
} ApiContext;

/* Global API state */
static ApiContext *global_api_context = NULL;

/* COMPLIANT: Safe API initialization with comprehensive validation */
ApiResult api_initialize(const char *session_id, size_t max_resources) {
    /* Validate parameters */
    if (!session_id) {
        errno = EINVAL;
        return API_ERROR_INVALID_PARAM;
    }

    size_t session_id_len = strlen(session_id);
    if (session_id_len == 0 || session_id_len > 64) {
        errno = EINVAL;
        return API_ERROR_INVALID_PARAM;
    }

    /* Validate session ID contains only safe characters */
    for (size_t i = 0; i < session_id_len; i++) {
        char c = session_id[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') || c == '-' || c == '_')) {
            errno = EINVAL;
            return API_ERROR_INVALID_PARAM;
        }
    }

    if (max_resources == 0 || max_resources > 10000) {
        errno = EINVAL;
        return API_ERROR_INVALID_PARAM;
    }

    /* Check if already initialized */
    if (global_api_context) {
        errno = EINVAL;
        return API_ERROR_INVALID_STATE;
    }

    /* Allocate API context */
    ApiContext *context = malloc(sizeof(ApiContext));
    if (!context) {
        errno = ENOMEM;
        return API_ERROR_OUT_OF_MEMORY;
    }

    /* Initialize context fields */
    memset(context, 0, sizeof(ApiContext));

    /* Allocate session ID copy */
    context->session_id = malloc(session_id_len + 1);
    if (!context->session_id) {
        free(context);
        errno = ENOMEM;
        return API_ERROR_OUT_OF_MEMORY;
    }
    strcpy(context->session_id, session_id);

    /* Allocate resource tracking array */
    context->allocated_resources = calloc(max_resources, sizeof(void *));
    if (!context->allocated_resources) {
        free(context->session_id);
        free(context);
        errno = ENOMEM;
        return API_ERROR_OUT_OF_MEMORY;
    }

    /* Complete initialization */
    context->is_initialized = 1;
    context->creation_time = time(NULL);
    context->operation_count = 0;
    context->resource_count = 0;
    context->max_resources = max_resources;
    strcpy(context->last_error, "API initialized successfully");

    /* Commit to global state */
    global_api_context = context;
    return API_SUCCESS;
}

/* COMPLIANT: Safe API context validation */
static ApiResult validate_api_context(void) {
    if (!global_api_context) {
        errno = EINVAL;
        return API_ERROR_INVALID_STATE;
    }

    if (!global_api_context->is_initialized) {
        errno = EINVAL;
        return API_ERROR_INVALID_STATE;
    }

    return API_SUCCESS;
}

/* COMPLIANT: Safe resource tracking */
static ApiResult track_resource(void *resource) {
    ApiResult validation = validate_api_context();
    if (validation != API_SUCCESS) {
        return validation;
    }

    if (!resource) {
        errno = EINVAL;
        return API_ERROR_INVALID_PARAM;
    }

    if (global_api_context->resource_count >= global_api_context->max_resources) {
        errno = ENOSPC;
        return API_ERROR_OUT_OF_RANGE;
    }

    /* Find free slot */
    for (size_t i = 0; i < global_api_context->max_resources; i++) {
        if (!global_api_context->allocated_resources[i]) {
            global_api_context->allocated_resources[i] = resource;
            global_api_context->resource_count++;
            return API_SUCCESS;
        }
    }

    errno = ENOSPC;
    return API_ERROR_OUT_OF_RANGE;
}

/* COMPLIANT: Safe resource untracking */
static ApiResult untrack_resource(void *resource) {
    ApiResult validation = validate_api_context();
    if (validation != API_SUCCESS) {
        return validation;
    }

    if (!resource) {
        errno = EINVAL;
        return API_ERROR_INVALID_PARAM;
    }

    /* Find and remove resource */
    for (size_t i = 0; i < global_api_context->max_resources; i++) {
        if (global_api_context->allocated_resources[i] == resource) {
            global_api_context->allocated_resources[i] = NULL;
            global_api_context->resource_count--;
            return API_SUCCESS;
        }
    }

    errno = ENOENT;
    return API_ERROR_INVALID_PARAM;
}

/* COMPLIANT: Safe memory allocation through API */
void *api_allocate_memory(size_t size) {
    /* Validate API state */
    if (validate_api_context() != API_SUCCESS) {
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "API not initialized");
        return NULL;
    }

    /* Validate size parameter */
    if (size == 0) {
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "Invalid allocation size: 0");
        return NULL;
    }

    const size_t MAX_ALLOCATION = 10 * 1024 * 1024;  /* 10 MB */
    if (size > MAX_ALLOCATION) {
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "Allocation size %zu exceeds maximum %zu", size, MAX_ALLOCATION);
        return NULL;
    }

    /* Attempt allocation */
    void *ptr = malloc(size);
    if (!ptr) {
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "Memory allocation failed for %zu bytes", size);
        return NULL;
    }

    /* Track resource */
    if (track_resource(ptr) != API_SUCCESS) {
        free(ptr);
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "Resource tracking failed");
        return NULL;
    }

    /* Clear allocated memory */
    memset(ptr, 0, size);

    /* Update operation count */
    global_api_context->operation_count++;

    snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
            "Allocated %zu bytes successfully", size);

    return ptr;
}

/* COMPLIANT: Safe memory deallocation through API */
ApiResult api_free_memory(void *ptr) {
    /* Validate API state */
    ApiResult validation = validate_api_context();
    if (validation != API_SUCCESS) {
        return validation;
    }

    /* Handle NULL pointer (safe operation) */
    if (!ptr) {
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "Attempted to free NULL pointer (safe operation)");
        return API_SUCCESS;
    }

    /* Untrack resource */
    if (untrack_resource(ptr) != API_SUCCESS) {
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "Pointer not tracked by API");
        return API_ERROR_INVALID_PARAM;
    }

    /* Free memory */
    free(ptr);

    /* Update operation count */
    global_api_context->operation_count++;

    snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
            "Memory freed successfully");

    return API_SUCCESS;
}

/* COMPLIANT: Safe string processing through API */
ApiResult api_process_string(const char *input, char **output, const char *operation) {
    /* Validate API state */
    ApiResult validation = validate_api_context();
    if (validation != API_SUCCESS) {
        return validation;
    }

    /* Validate parameters */
    if (!input || !output || !operation) {
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "NULL parameter in string processing");
        return API_ERROR_INVALID_PARAM;
    }

    /* Initialize output */
    *output = NULL;

    size_t input_len = strlen(input);
    size_t operation_len = strlen(operation);

    /* Validate string lengths */
    const size_t MAX_STRING_LEN = 1024 * 1024;  /* 1 MB */
    if (input_len > MAX_STRING_LEN) {
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "Input string too long: %zu characters", input_len);
        return API_ERROR_OUT_OF_RANGE;
    }

    if (operation_len == 0 || operation_len > 32) {
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "Invalid operation name length: %zu", operation_len);
        return API_ERROR_INVALID_PARAM;
    }

    /* Allocate output buffer */
    char *result = api_allocate_memory(input_len + 1);
    if (!result) {
        return API_ERROR_OUT_OF_MEMORY;
    }

    /* Process based on operation */
    if (strcmp(operation, "copy") == 0) {
        strcpy(result, input);
    } else if (strcmp(operation, "upper") == 0) {
        for (size_t i = 0; i < input_len; i++) {
            result[i] = (char)toupper((unsigned char)input[i]);
        }
        result[input_len] = '\0';
    } else if (strcmp(operation, "lower") == 0) {
        for (size_t i = 0; i < input_len; i++) {
            result[i] = (char)tolower((unsigned char)input[i]);
        }
        result[input_len] = '\0';
    } else {
        api_free_memory(result);
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "Unknown operation: %s", operation);
        return API_ERROR_INVALID_PARAM;
    }

    /* Success - commit result */
    *output = result;
    global_api_context->operation_count++;

    snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
            "String processing (%s) completed successfully", operation);

    return API_SUCCESS;
}

/* COMPLIANT: Safe variadic function with validation */
ApiResult api_format_message(char **output, const char *format, ...) {
    /* Validate API state */
    ApiResult validation = validate_api_context();
    if (validation != API_SUCCESS) {
        return validation;
    }

    /* Validate parameters */
    if (!output || !format) {
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "NULL parameter in format message");
        return API_ERROR_INVALID_PARAM;
    }

    /* Initialize output */
    *output = NULL;

    /* Validate format string */
    size_t format_len = strlen(format);
    if (format_len == 0 || format_len > 1024) {
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "Invalid format string length: %zu", format_len);
        return API_ERROR_INVALID_PARAM;
    }

    /* Count format specifiers to estimate buffer size */
    int specifier_count = 0;
    for (size_t i = 0; i < format_len - 1; i++) {
        if (format[i] == '%' && format[i + 1] != '%') {
            specifier_count++;
        }
    }

    /* Validate reasonable number of specifiers */
    if (specifier_count > 32) {
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "Too many format specifiers: %d", specifier_count);
        return API_ERROR_OUT_OF_RANGE;
    }

    /* Allocate buffer with conservative size estimate */
    const size_t base_size = format_len;
    const size_t per_specifier_size = 64;  /* Conservative estimate per specifier */
    size_t buffer_size = base_size + (specifier_count * per_specifier_size) + 1;

    char *buffer = api_allocate_memory(buffer_size);
    if (!buffer) {
        return API_ERROR_OUT_OF_MEMORY;
    }

    /* Format message using variadic arguments */
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, buffer_size, format, args);
    va_end(args);

    /* Check formatting result */
    if (result < 0) {
        api_free_memory(buffer);
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "Format operation failed");
        return API_ERROR_OPERATION_FAILED;
    }

    if ((size_t)result >= buffer_size) {
        api_free_memory(buffer);
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "Formatted message too long: %d characters", result);
        return API_ERROR_OUT_OF_RANGE;
    }

    /* Success - commit result */
    *output = buffer;
    global_api_context->operation_count++;

    snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
            "Message formatted successfully (%d characters)", result);

    return API_SUCCESS;
}

/* COMPLIANT: Safe API status reporting */
ApiResult api_get_status(char **status_report) {
    /* Validate API state */
    ApiResult validation = validate_api_context();
    if (validation != API_SUCCESS) {
        return validation;
    }

    /* Validate parameter */
    if (!status_report) {
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "NULL status report parameter");
        return API_ERROR_INVALID_PARAM;
    }

    /* Initialize output */
    *status_report = NULL;

    /* Allocate status buffer */
    const size_t STATUS_BUFFER_SIZE = 1024;
    char *status = api_allocate_memory(STATUS_BUFFER_SIZE);
    if (!status) {
        return API_ERROR_OUT_OF_MEMORY;
    }

    /* Format status report */
    time_t current_time = time(NULL);
    double uptime = difftime(current_time, global_api_context->creation_time);

    int written = snprintf(status, STATUS_BUFFER_SIZE,
        "API Status Report:\n"
        "  Session ID: %s\n"
        "  Initialized: %s\n"
        "  Uptime: %.0f seconds\n"
        "  Operations: %zu\n"
        "  Active Resources: %zu / %zu\n"
        "  Last Error: %s\n",
        global_api_context->session_id,
        global_api_context->is_initialized ? "Yes" : "No",
        uptime,
        global_api_context->operation_count,
        global_api_context->resource_count,
        global_api_context->max_resources,
        global_api_context->last_error);

    if (written < 0 || (size_t)written >= STATUS_BUFFER_SIZE) {
        api_free_memory(status);
        snprintf(global_api_context->last_error, sizeof(global_api_context->last_error),
                "Status report formatting failed");
        return API_ERROR_OPERATION_FAILED;
    }

    /* Success - commit result */
    *status_report = status;
    global_api_context->operation_count++;

    return API_SUCCESS;
}

/* COMPLIANT: Safe API cleanup with resource management */
ApiResult api_cleanup(void) {
    /* Validate API state */
    if (!global_api_context) {
        return API_SUCCESS;  /* Already cleaned up */
    }

    /* Free all tracked resources */
    if (global_api_context->allocated_resources) {
        for (size_t i = 0; i < global_api_context->max_resources; i++) {
            if (global_api_context->allocated_resources[i]) {
                free(global_api_context->allocated_resources[i]);
            }
        }
        free(global_api_context->allocated_resources);
    }

    /* Free session ID */
    free(global_api_context->session_id);

    /* Free context */
    free(global_api_context);
    global_api_context = NULL;

    return API_SUCCESS;
}

int main(void) {
    printf("=== Comprehensive API Validation Demo ===\n\n");

    /* Test API initialization */
    printf("1. API Initialization:\n");
    ApiResult init_result = api_initialize("test-session-123", 100);
    if (init_result == API_SUCCESS) {
        printf("   API initialized successfully\n");
    } else {
        printf("   API initialization failed: %d\n", init_result);
        return 1;
    }

    /* Test parameter validation in initialization */
    printf("\n2. Parameter validation tests:\n");
    ApiResult invalid_init = api_initialize(NULL, 100);
    if (invalid_init != API_SUCCESS) {
        printf("   Correctly rejected NULL session ID\n");
    }

    /* Test memory operations */
    printf("\n3. Memory operations:\n");
    void *mem1 = api_allocate_memory(1024);
    if (mem1) {
        printf("   Allocated 1024 bytes successfully\n");

        if (api_free_memory(mem1) == API_SUCCESS) {
            printf("   Freed memory successfully\n");
        }
    }

    /* Test invalid memory operations */
    void *invalid_mem = api_allocate_memory(0);
    if (!invalid_mem) {
        printf("   Correctly rejected zero-size allocation\n");
    }

    /* Test string processing */
    printf("\n4. String processing:\n");
    char *processed_string = NULL;
    ApiResult process_result = api_process_string("Hello World", &processed_string, "upper");
    if (process_result == API_SUCCESS && processed_string) {
        printf("   String processing successful: %s\n", processed_string);
        api_free_memory(processed_string);
    } else {
        printf("   String processing failed: %d\n", process_result);
    }

    /* Test invalid string operation */
    char *invalid_processed = NULL;
    ApiResult invalid_process = api_process_string("test", &invalid_processed, "invalid_op");
    if (invalid_process != API_SUCCESS) {
        printf("   Correctly rejected invalid operation\n");
    }

    /* Test variadic function */
    printf("\n5. Variadic function:\n");
    char *formatted_msg = NULL;
    ApiResult format_result = api_format_message(&formatted_msg, "User %s has %d points", "Alice", 100);
    if (format_result == API_SUCCESS && formatted_msg) {
        printf("   Formatted message: %s\n", formatted_msg);
        api_free_memory(formatted_msg);
    }

    /* Test status reporting */
    printf("\n6. Status reporting:\n");
    char *status = NULL;
    ApiResult status_result = api_get_status(&status);
    if (status_result == API_SUCCESS && status) {
        printf("%s\n", status);
        api_free_memory(status);
    }

    /* Test cleanup */
    printf("\n7. API cleanup:\n");
    if (api_cleanup() == API_SUCCESS) {
        printf("   API cleaned up successfully\n");
    }

    printf("\n=== API validation demo completed ===\n");
    return 0;
}