/*
 * Rule: API00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger API00-C violation
 */

/*
 * CERT C API00-C Pass Case: robust_file_operations.c
 *
 * This case demonstrates compliant file operations with comprehensive
 * parameter validation and proper error handling.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

/* File operation result structure */
typedef struct {
    int success;
    size_t bytes_processed;
    char error_message[256];
} FileResult;

/* COMPLIANT: Safe file existence check */
int safe_file_exists(const char *filename) {
    /* Validate parameter */
    if (!filename) {
        errno = EINVAL;
        return -1;
    }

    /* Check for empty filename */
    if (strlen(filename) == 0) {
        errno = EINVAL;
        return -1;
    }

    /* Use access() to check file existence */
    return access(filename, F_OK) == 0 ? 1 : 0;
}

/* COMPLIANT: Safe file size determination */
int safe_get_file_size(const char *filename, size_t *file_size) {
    /* Validate parameters */
    if (!filename || !file_size) {
        errno = EINVAL;
        return -1;
    }

    if (strlen(filename) == 0) {
        errno = EINVAL;
        return -1;
    }

    /* Get file statistics */
    struct stat file_stat;
    if (stat(filename, &file_stat) != 0) {
        /* errno set by stat() */
        return -1;
    }

    /* Ensure it's a regular file */
    if (!S_ISREG(file_stat.st_mode)) {
        errno = EISDIR;  /* Or other appropriate error */
        return -1;
    }

    /* Check for reasonable file size */
    const off_t MAX_SAFE_SIZE = 100 * 1024 * 1024;  /* 100 MB */
    if (file_stat.st_size > MAX_SAFE_SIZE) {
        errno = EFBIG;
        return -1;
    }

    *file_size = (size_t)file_stat.st_size;
    return 0;
}

/* COMPLIANT: Safe text file reading with validation */
FileResult safe_read_text_file(const char *filename, char **content) {
    FileResult result = {0, 0, ""};

    /* Validate parameters */
    if (!filename || !content) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid parameters: filename or content pointer is NULL");
        return result;
    }

    /* Initialize output */
    *content = NULL;

    /* Check file exists and get size */
    size_t file_size;
    if (safe_get_file_size(filename, &file_size) != 0) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Cannot access file '%s': %s", filename, strerror(errno));
        return result;
    }

    /* Allocate buffer with extra space for null terminator */
    char *buffer = malloc(file_size + 1);
    if (!buffer) {
        errno = ENOMEM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Cannot allocate %zu bytes: %s", file_size + 1, strerror(errno));
        return result;
    }

    /* Open file */
    FILE *file = fopen(filename, "rb");
    if (!file) {
        free(buffer);
        snprintf(result.error_message, sizeof(result.error_message),
                "Cannot open file '%s': %s", filename, strerror(errno));
        return result;
    }

    /* Read file contents */
    size_t bytes_read = fread(buffer, 1, file_size, file);
    fclose(file);

    /* Verify we read the expected amount */
    if (bytes_read != file_size) {
        free(buffer);
        snprintf(result.error_message, sizeof(result.error_message),
                "Read %zu bytes, expected %zu", bytes_read, file_size);
        return result;
    }

    /* Null-terminate the buffer */
    buffer[file_size] = '\0';

    /* Success - commit results */
    *content = buffer;
    result.success = 1;
    result.bytes_processed = bytes_read;
    snprintf(result.error_message, sizeof(result.error_message),
            "Successfully read %zu bytes", bytes_read);

    return result;
}

/* COMPLIANT: Safe text file writing with validation */
FileResult safe_write_text_file(const char *filename, const char *content) {
    FileResult result = {0, 0, ""};

    /* Validate parameters */
    if (!filename || !content) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid parameters: filename or content is NULL");
        return result;
    }

    if (strlen(filename) == 0) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Filename cannot be empty");
        return result;
    }

    size_t content_length = strlen(content);

    /* Check for reasonable content size */
    const size_t MAX_WRITE_SIZE = 50 * 1024 * 1024;  /* 50 MB */
    if (content_length > MAX_WRITE_SIZE) {
        errno = EFBIG;
        snprintf(result.error_message, sizeof(result.error_message),
                "Content size %zu exceeds maximum %zu", content_length, MAX_WRITE_SIZE);
        return result;
    }

    /* Open file for writing */
    FILE *file = fopen(filename, "wb");
    if (!file) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Cannot create file '%s': %s", filename, strerror(errno));
        return result;
    }

    /* Write content */
    size_t bytes_written = fwrite(content, 1, content_length, file);

    /* Check for write errors before closing */
    int write_error = ferror(file);
    fclose(file);

    if (write_error || bytes_written != content_length) {
        /* Remove partially written file */
        unlink(filename);
        snprintf(result.error_message, sizeof(result.error_message),
                "Write failed: wrote %zu of %zu bytes", bytes_written, content_length);
        return result;
    }

    /* Success */
    result.success = 1;
    result.bytes_processed = bytes_written;
    snprintf(result.error_message, sizeof(result.error_message),
            "Successfully wrote %zu bytes", bytes_written);

    return result;
}

/* COMPLIANT: Safe file copying with validation */
FileResult safe_copy_file(const char *source_path, const char *dest_path) {
    FileResult result = {0, 0, ""};

    /* Validate parameters */
    if (!source_path || !dest_path) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid parameters: source or destination path is NULL");
        return result;
    }

    if (strlen(source_path) == 0 || strlen(dest_path) == 0) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Source and destination paths cannot be empty");
        return result;
    }

    /* Check that source and destination are different */
    if (strcmp(source_path, dest_path) == 0) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Source and destination cannot be the same");
        return result;
    }

    /* Verify source file exists and get its size */
    size_t source_size;
    if (safe_get_file_size(source_path, &source_size) != 0) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Cannot access source file '%s': %s", source_path, strerror(errno));
        return result;
    }

    /* Open source file */
    FILE *source_file = fopen(source_path, "rb");
    if (!source_file) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Cannot open source file '%s': %s", source_path, strerror(errno));
        return result;
    }

    /* Open destination file */
    FILE *dest_file = fopen(dest_path, "wb");
    if (!dest_file) {
        fclose(source_file);
        snprintf(result.error_message, sizeof(result.error_message),
                "Cannot create destination file '%s': %s", dest_path, strerror(errno));
        return result;
    }

    /* Copy data in chunks */
    const size_t BUFFER_SIZE = 8192;
    char buffer[BUFFER_SIZE];
    size_t total_copied = 0;
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, source_file)) > 0) {
        size_t bytes_written = fwrite(buffer, 1, bytes_read, dest_file);

        if (bytes_written != bytes_read || ferror(dest_file)) {
            /* Copy failed - clean up */
            fclose(source_file);
            fclose(dest_file);
            unlink(dest_path);  /* Remove partial file */

            snprintf(result.error_message, sizeof(result.error_message),
                    "Copy failed at byte %zu: write error", total_copied);
            return result;
        }

        total_copied += bytes_written;
    }

    /* Check for read errors */
    if (ferror(source_file)) {
        fclose(source_file);
        fclose(dest_file);
        unlink(dest_path);

        snprintf(result.error_message, sizeof(result.error_message),
                "Copy failed: read error from source file");
        return result;
    }

    fclose(source_file);
    fclose(dest_file);

    /* Verify we copied the expected amount */
    if (total_copied != source_size) {
        unlink(dest_path);
        snprintf(result.error_message, sizeof(result.error_message),
                "Copy incomplete: copied %zu of %zu bytes", total_copied, source_size);
        return result;
    }

    /* Success */
    result.success = 1;
    result.bytes_processed = total_copied;
    snprintf(result.error_message, sizeof(result.error_message),
            "Successfully copied %zu bytes", total_copied);

    return result;
}

/* COMPLIANT: Safe file deletion with validation */
int safe_delete_file(const char *filename) {
    /* Validate parameter */
    if (!filename) {
        errno = EINVAL;
        return -1;
    }

    if (strlen(filename) == 0) {
        errno = EINVAL;
        return -1;
    }

    /* Check if file exists first */
    if (safe_file_exists(filename) != 1) {
        errno = ENOENT;
        return -1;
    }

    /* Attempt deletion */
    if (unlink(filename) != 0) {
        /* errno set by unlink */
        return -1;
    }

    return 0;
}

int main(void) {
    printf("=== Robust File Operations Demo ===\n\n");

    const char *test_filename = "test_file.txt";
    const char *copy_filename = "test_file_copy.txt";
    const char *test_content = "This is a test file.\nIt contains multiple lines.\nFor testing file operations.\n";

    /* Test writing a file */
    printf("1. Writing test file...\n");
    FileResult write_result = safe_write_text_file(test_filename, test_content);
    if (write_result.success) {
        printf("   %s\n", write_result.error_message);
    } else {
        printf("   Error: %s\n", write_result.error_message);
        return 1;
    }

    /* Test reading the file back */
    printf("2. Reading test file...\n");
    char *read_content;
    FileResult read_result = safe_read_text_file(test_filename, &read_content);
    if (read_result.success) {
        printf("   %s\n", read_result.error_message);
        printf("   Content preview: %.50s%s\n", read_content,
               strlen(read_content) > 50 ? "..." : "");
        free(read_content);
    } else {
        printf("   Error: %s\n", read_result.error_message);
    }

    /* Test file size */
    printf("3. Getting file size...\n");
    size_t file_size;
    if (safe_get_file_size(test_filename, &file_size) == 0) {
        printf("   File size: %zu bytes\n", file_size);
    } else {
        printf("   Error getting file size: %s\n", strerror(errno));
    }

    /* Test copying the file */
    printf("4. Copying file...\n");
    FileResult copy_result = safe_copy_file(test_filename, copy_filename);
    if (copy_result.success) {
        printf("   %s\n", copy_result.error_message);
    } else {
        printf("   Error: %s\n", copy_result.error_message);
    }

    /* Test parameter validation with NULL */
    printf("5. Testing NULL parameter handling...\n");
    FileResult null_test = safe_read_text_file(NULL, &read_content);
    if (!null_test.success) {
        printf("   Correctly rejected NULL parameter: %s\n", null_test.error_message);
    }

    /* Test non-existent file */
    printf("6. Testing non-existent file...\n");
    FileResult missing_test = safe_read_text_file("non_existent_file.txt", &read_content);
    if (!missing_test.success) {
        printf("   Correctly handled missing file: %s\n", missing_test.error_message);
    }

    /* Clean up test files */
    printf("7. Cleaning up...\n");
    if (safe_delete_file(test_filename) == 0) {
        printf("   Deleted test file\n");
    }
    if (safe_delete_file(copy_filename) == 0) {
        printf("   Deleted copy file\n");
    }

    printf("\n=== File operations demo completed ===\n");
    return 0;
}