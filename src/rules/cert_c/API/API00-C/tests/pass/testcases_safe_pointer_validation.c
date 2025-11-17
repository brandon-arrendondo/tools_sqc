/*
 * Rule: API00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger API00-C violation
 */

/*
 * CERT C API00-C Pass Case: safe_pointer_validation.c
 *
 * This case demonstrates compliant code that properly validates
 * pointer parameters before use, following commit-or-rollback semantics.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Global state that should remain unchanged on error */
static char *global_message = NULL;

/* COMPLIANT: Function validates pointer parameters and implements error handling */
int safe_string_copy(char *dest, size_t dest_size, const char *src) {
    /* Validate all parameters before any operations */
    if (!dest || !src) {
        errno = EINVAL;
        return -1;  /* Error: leave all state unchanged */
    }

    if (dest_size == 0) {
        errno = EINVAL;
        return -1;
    }

    /* Check if source string will fit in destination */
    size_t src_len = strlen(src);
    if (src_len >= dest_size) {
        errno = ENOSPC;
        return -1;  /* Error: leave destination unchanged */
    }

    /* All validation passed - now perform the operation */
    strcpy(dest, src);
    return 0;  /* Success */
}

/* COMPLIANT: Array function with comprehensive bounds checking */
int safe_array_sum(const int *array, size_t size, long *result) {
    /* Validate parameters */
    if (!array || !result) {
        errno = EINVAL;
        return -1;
    }

    if (size == 0) {
        *result = 0;  /* Valid operation for empty array */
        return 0;
    }

    /* Check for potential overflow */
    long sum = 0;
    for (size_t i = 0; i < size; i++) {
        /* Check for overflow before adding */
        if ((array[i] > 0 && sum > LONG_MAX - array[i]) ||
            (array[i] < 0 && sum < LONG_MIN - array[i])) {
            errno = ERANGE;
            return -1;  /* Error: leave result unchanged */
        }
        sum += array[i];
    }

    *result = sum;
    return 0;
}

/* COMPLIANT: File operation with comprehensive validation */
int safe_file_read(const char *filename, char **buffer, size_t *size) {
    /* Validate parameters */
    if (!filename || !buffer || !size) {
        errno = EINVAL;
        return -1;
    }

    if (strlen(filename) == 0) {
        errno = EINVAL;
        return -1;
    }

    /* Attempt to open file */
    FILE *file = fopen(filename, "rb");
    if (!file) {
        /* errno set by fopen */
        return -1;
    }

    /* Get file size */
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return -1;
    }

    long file_size = ftell(file);
    if (file_size < 0) {
        fclose(file);
        return -1;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return -1;
    }

    /* Validate file size is reasonable */
    const size_t MAX_FILE_SIZE = 10 * 1024 * 1024;  /* 10 MB limit */
    if ((size_t)file_size > MAX_FILE_SIZE) {
        fclose(file);
        errno = EFBIG;
        return -1;
    }

    /* Allocate buffer */
    char *temp_buffer = malloc((size_t)file_size + 1);
    if (!temp_buffer) {
        fclose(file);
        errno = ENOMEM;
        return -1;
    }

    /* Read file contents */
    size_t bytes_read = fread(temp_buffer, 1, (size_t)file_size, file);
    fclose(file);

    if (bytes_read != (size_t)file_size) {
        free(temp_buffer);
        errno = EIO;
        return -1;
    }

    temp_buffer[file_size] = '\0';  /* Null terminate */

    /* All operations successful - commit results */
    *buffer = temp_buffer;
    *size = (size_t)file_size;
    return 0;
}

/* COMPLIANT: Memory operation with validation and rollback */
int safe_memory_copy(void *dest, size_t dest_size, const void *src, size_t src_size) {
    /* Validate parameters */
    if (!dest || !src) {
        errno = EINVAL;
        return -1;
    }

    if (dest_size == 0 || src_size == 0) {
        errno = EINVAL;
        return -1;
    }

    /* Validate destination has enough space */
    if (src_size > dest_size) {
        errno = ENOSPC;
        return -1;
    }

    /* Check for overlap in potentially overlapping memory regions */
    const char *src_bytes = (const char *)src;
    char *dest_bytes = (char *)dest;

    if ((src_bytes < dest_bytes && src_bytes + src_size > dest_bytes) ||
        (dest_bytes < src_bytes && dest_bytes + dest_size > src_bytes)) {
        /* Use memmove for overlapping regions */
        memmove(dest, src, src_size);
    } else {
        /* Safe to use memcpy */
        memcpy(dest, src, src_size);
    }

    return 0;
}

/* COMPLIANT: Function with state preservation on error */
int safe_set_global_message(const char *new_message) {
    /* Validate parameter */
    if (!new_message) {
        errno = EINVAL;
        return -1;
    }

    /* Validate message length */
    size_t msg_len = strlen(new_message);
    const size_t MAX_MESSAGE_LEN = 1024;
    if (msg_len > MAX_MESSAGE_LEN) {
        errno = EINVAL;
        return -1;
    }

    /* Attempt to allocate new message */
    char *temp_message = malloc(msg_len + 1);
    if (!temp_message) {
        errno = ENOMEM;
        return -1;  /* Global state unchanged */
    }

    strcpy(temp_message, new_message);

    /* All validation passed - commit the change */
    free(global_message);  /* Safe to call on NULL */
    global_message = temp_message;
    return 0;
}

/* COMPLIANT: Division function with zero checking */
int safe_divide(double dividend, double divisor, double *result) {
    /* Validate parameters */
    if (!result) {
        errno = EINVAL;
        return -1;
    }

    /* Check for division by zero */
    if (divisor == 0.0) {
        errno = EDOM;
        return -1;  /* Leave result unchanged */
    }

    /* Check for potential overflow/underflow */
    if (dividend != 0.0) {
        double abs_dividend = fabs(dividend);
        double abs_divisor = fabs(divisor);

        if (abs_dividend / abs_divisor > DBL_MAX) {
            errno = ERANGE;
            return -1;
        }
    }

    *result = dividend / divisor;
    return 0;
}

/* COMPLIANT: String processing with validation */
int safe_string_to_int(const char *str, int *result) {
    /* Validate parameters */
    if (!str || !result) {
        errno = EINVAL;
        return -1;
    }

    /* Check for empty string */
    if (strlen(str) == 0) {
        errno = EINVAL;
        return -1;
    }

    /* Use strtol for safe conversion */
    char *endptr;
    errno = 0;  /* Reset errno before conversion */
    long temp_result = strtol(str, &endptr, 10);

    /* Check for conversion errors */
    if (errno == ERANGE) {
        return -1;  /* Overflow/underflow */
    }

    if (endptr == str) {
        errno = EINVAL;
        return -1;  /* No conversion performed */
    }

    if (*endptr != '\0') {
        errno = EINVAL;
        return -1;  /* Extra characters after number */
    }

    /* Check if result fits in int */
    if (temp_result > INT_MAX || temp_result < INT_MIN) {
        errno = ERANGE;
        return -1;
    }

    *result = (int)temp_result;
    return 0;
}

int main(void) {
    printf("=== Safe Parameter Validation Examples ===\n\n");

    /* Demonstrate safe string copying */
    char buffer[100];
    if (safe_string_copy(buffer, sizeof(buffer), "Hello, World!") == 0) {
        printf("String copy successful: %s\n", buffer);
    } else {
        printf("String copy failed: %s\n", strerror(errno));
    }

    /* Test with NULL parameter */
    if (safe_string_copy(NULL, 100, "test") == 0) {
        printf("Unexpected success with NULL parameter\n");
    } else {
        printf("Correctly rejected NULL parameter: %s\n", strerror(errno));
    }

    /* Demonstrate safe array operations */
    int numbers[] = {1, 2, 3, 4, 5};
    long sum;
    if (safe_array_sum(numbers, 5, &sum) == 0) {
        printf("Array sum: %ld\n", sum);
    } else {
        printf("Array sum failed: %s\n", strerror(errno));
    }

    /* Demonstrate safe division */
    double result;
    if (safe_divide(10.0, 3.0, &result) == 0) {
        printf("Division result: %.2f\n", result);
    } else {
        printf("Division failed: %s\n", strerror(errno));
    }

    /* Test division by zero */
    if (safe_divide(10.0, 0.0, &result) == 0) {
        printf("Unexpected success with division by zero\n");
    } else {
        printf("Correctly rejected division by zero: %s\n", strerror(errno));
    }

    /* Demonstrate safe string to int conversion */
    int int_result;
    if (safe_string_to_int("42", &int_result) == 0) {
        printf("String to int conversion: %d\n", int_result);
    } else {
        printf("String to int conversion failed: %s\n", strerror(errno));
    }

    /* Clean up global state */
    free(global_message);
    global_message = NULL;

    printf("\n=== All parameter validation tests completed ===\n");
    return 0;
}