/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Pass Case: const_file_operations.c
 *
 * This case demonstrates compliant code that properly uses const
 * qualification in file operations and I/O scenarios.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* COMPLIANT: File operation constants */
static const char * const DEFAULT_OUTPUT_FILE = "output.txt";
static const char * const TEMP_FILE_PREFIX = "temp_";
static const char * const LOG_FILE_EXTENSION = ".log";
static const size_t MAX_LINE_LENGTH = 1024;
static const size_t MAX_FILENAME_LENGTH = 256;

/* COMPLIANT: File mode constants */
static const char * const FILE_MODE_READ = "r";
static const char * const FILE_MODE_WRITE = "w";
static const char * const FILE_MODE_APPEND = "a";
static const char * const FILE_MODE_READ_BINARY = "rb";
static const char * const FILE_MODE_WRITE_BINARY = "wb";

/* COMPLIANT: Function with const filename parameter */
int write_text_to_file(const char *filename, const char *content) {
    if (!filename || !content) {
        printf("Error: NULL parameter provided\\n");
        return -1;
    }

    /* COMPLIANT: Local const for operation description */
    const char * const OPERATION = "write_text_to_file";

    printf("%s: Writing to '%s'\\n", OPERATION, filename);

    FILE *file = fopen(filename, FILE_MODE_WRITE);
    if (!file) {
        printf("Error: Cannot open file '%s' for writing\\n", filename);
        return -1;
    }

    /* COMPLIANT: Using const content parameter */
    const size_t content_length = strlen(content);
    const size_t written = fwrite(content, 1, content_length, file);

    fclose(file);

    if (written == content_length) {
        printf("Successfully wrote %zu bytes\\n", written);
        return 0;
    } else {
        printf("Error: Only wrote %zu of %zu bytes\\n", written, content_length);
        return -1;
    }
}

/* COMPLIANT: Function with const parameters for file reading */
char *read_text_from_file(const char *filename) {
    if (!filename) {
        return NULL;
    }

    /* COMPLIANT: Local const for operation tracking */
    const char * const OPERATION = "read_text_from_file";

    printf("%s: Reading from '%s'\\n", OPERATION, filename);

    FILE *file = fopen(filename, FILE_MODE_READ);
    if (!file) {
        printf("Error: Cannot open file '%s' for reading\\n", filename);
        return NULL;
    }

    /* Determine file size */
    fseek(file, 0, SEEK_END);
    const long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size < 0) {
        printf("Error: Cannot determine file size\\n");
        fclose(file);
        return NULL;
    }

    /* Allocate buffer */
    const size_t buffer_size = (size_t)file_size + 1;
    char *buffer = malloc(buffer_size);
    if (!buffer) {
        printf("Error: Cannot allocate %zu bytes\\n", buffer_size);
        fclose(file);
        return NULL;
    }

    /* Read file content */
    const size_t bytes_read = fread(buffer, 1, (size_t)file_size, file);
    buffer[bytes_read] = '\\0';

    fclose(file);

    printf("Successfully read %zu bytes\\n", bytes_read);
    return buffer;
}

/* COMPLIANT: Function for processing CSV files with const parameters */
void process_csv_file(const char *filename, const char delimiter) {
    if (!filename) {
        printf("Error: NULL filename\\n");
        return;
    }

    /* COMPLIANT: Local const for CSV processing */
    const char * const OPERATION = "process_csv_file";
    const size_t MAX_FIELDS = 10;

    printf("\\n%s: Processing '%s' with delimiter '%c'\\n",
           OPERATION, filename, delimiter);

    FILE *file = fopen(filename, FILE_MODE_READ);
    if (!file) {
        printf("Error: Cannot open CSV file '%s'\\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int line_number = 0;

    while (fgets(line, sizeof(line), file)) {
        line_number++;

        /* Remove newline if present */
        const size_t line_len = strlen(line);
        if (line_len > 0 && line[line_len - 1] == '\\n') {
            line[line_len - 1] = '\\0';
        }

        printf("  Line %d: %s\\n", line_number, line);

        /* Parse fields using const delimiter */
        char *field = strtok(line, &delimiter);
        int field_count = 0;

        while (field && field_count < (int)MAX_FIELDS) {
            printf("    Field %d: '%s'\\n", field_count + 1, field);
            field = strtok(NULL, &delimiter);
            field_count++;
        }
    }

    fclose(file);
    printf("Processed %d lines\\n", line_number);
}

/* COMPLIANT: Function for logging with const formatting */
void write_log_entry(const char *log_filename, const char *level,
                     const char *message) {
    if (!log_filename || !level || !message) {
        return;
    }

    /* COMPLIANT: Const format strings */
    const char * const TIMESTAMP_FORMAT = "%Y-%m-%d %H:%M:%S";
    const char * const LOG_ENTRY_FORMAT = "[%s] %s: %s\\n";

    FILE *log_file = fopen(log_filename, FILE_MODE_APPEND);
    if (!log_file) {
        printf("Warning: Cannot open log file '%s'\\n", log_filename);
        return;
    }

    /* Get current time */
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);

    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), TIMESTAMP_FORMAT, local_time);

    /* Write log entry using const format */
    fprintf(log_file, LOG_ENTRY_FORMAT, timestamp, level, message);

    fclose(log_file);
}

/* COMPLIANT: Configuration file processing with const parameters */
void process_config_file(const char *config_filename) {
    if (!config_filename) {
        return;
    }

    /* COMPLIANT: Const configuration parsing parameters */
    const char COMMENT_CHAR = '#';
    const char ASSIGNMENT_CHAR = '=';
    const char * const SECTION_START = "[";
    const char * const SECTION_END = "]";

    printf("\\nProcessing configuration file: %s\\n", config_filename);

    FILE *config_file = fopen(config_filename, FILE_MODE_READ);
    if (!config_file) {
        printf("Error: Cannot open config file '%s'\\n", config_filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    char current_section[64] = "default";

    while (fgets(line, sizeof(line), config_file)) {
        line_number++;

        /* Remove trailing newline */
        const size_t line_len = strlen(line);
        if (line_len > 0 && line[line_len - 1] == '\\n') {
            line[line_len - 1] = '\\0';
        }

        /* Skip empty lines and comments */
        if (line_len == 0 || line[0] == COMMENT_CHAR) {
            continue;
        }

        /* Check for section header */
        if (line[0] == SECTION_START[0] && line[line_len - 1] == SECTION_END[0]) {
            strncpy(current_section, line + 1, sizeof(current_section) - 1);
            current_section[sizeof(current_section) - 1] = '\\0';
            /* Remove closing bracket */
            char *bracket = strchr(current_section, SECTION_END[0]);
            if (bracket) *bracket = '\\0';

            printf("  Section: [%s]\\n", current_section);
            continue;
        }

        /* Parse key=value pairs */
        char *assignment = strchr(line, ASSIGNMENT_CHAR);
        if (assignment) {
            *assignment = '\\0';  /* Split at assignment character */
            const char *key = line;
            const char *value = assignment + 1;

            printf("    %s.%s = %s\\n", current_section, key, value);
        }
    }

    fclose(config_file);
    printf("Configuration file processing completed\\n");
}

/* COMPLIANT: Binary file operations with const parameters */
void demonstrate_binary_file_ops(const char *binary_filename) {
    if (!binary_filename) {
        return;
    }

    /* COMPLIANT: Const binary data */
    const unsigned char binary_data[] = {
        0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A,  /* PNG signature */
        0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,  /* IHDR chunk */
        0x00, 0x01, 0x00, 0x01, 0x08, 0x02, 0x00, 0x00
    };
    const size_t data_size = sizeof(binary_data);

    printf("\\nBinary file operations with '%s'\\n", binary_filename);

    /* Write binary data */
    FILE *bin_file = fopen(binary_filename, FILE_MODE_WRITE_BINARY);
    if (!bin_file) {
        printf("Error: Cannot create binary file\\n");
        return;
    }

    const size_t written = fwrite(binary_data, 1, data_size, bin_file);
    fclose(bin_file);

    printf("Written %zu bytes of binary data\\n", written);

    /* Read binary data back */
    bin_file = fopen(binary_filename, FILE_MODE_READ_BINARY);
    if (!bin_file) {
        printf("Error: Cannot read binary file\\n");
        return;
    }

    unsigned char read_buffer[sizeof(binary_data)];
    const size_t bytes_read = fread(read_buffer, 1, sizeof(read_buffer), bin_file);
    fclose(bin_file);

    printf("Read %zu bytes of binary data\\n", bytes_read);

    /* Compare data */
    if (bytes_read == data_size && memcmp(binary_data, read_buffer, data_size) == 0) {
        printf("Binary data verification: SUCCESS\\n");
    } else {
        printf("Binary data verification: FAILED\\n");
    }

    /* Clean up */
    remove(binary_filename);
}

int main(void) {
    /* COMPLIANT: Main function const declarations */
    const char * const PROGRAM_TITLE = "Const File Operations Demo";
    const char * const TEST_TEXT_FILE = "test_output.txt";
    const char * const TEST_CSV_FILE = "test_data.csv";
    const char * const TEST_LOG_FILE = "application.log";
    const char * const TEST_CONFIG_FILE = "config.ini";
    const char * const TEST_BINARY_FILE = "test_binary.dat";

    printf("=== %s ===\\n\\n", PROGRAM_TITLE);

    /* COMPLIANT: Test content as const */
    const char * const test_content =
        "This is a test file content.\\n"
        "It demonstrates const-qualified file operations.\\n"
        "Multiple lines are supported.\\n";

    /* Test text file operations */
    if (write_text_to_file(TEST_TEXT_FILE, test_content) == 0) {
        char *read_content = read_text_from_file(TEST_TEXT_FILE);
        if (read_content) {
            printf("\\nFile content verification:\\n%s\\n", read_content);
            free(read_content);
        }
        remove(TEST_TEXT_FILE);
    }

    /* Create and process CSV file */
    const char * const csv_content = "Name,Age,City\\nJohn,25,NYC\\nJane,30,LA\\nBob,35,Chicago\\n";
    if (write_text_to_file(TEST_CSV_FILE, csv_content) == 0) {
        process_csv_file(TEST_CSV_FILE, ',');
        remove(TEST_CSV_FILE);
    }

    /* Test logging */
    write_log_entry(TEST_LOG_FILE, "INFO", "Application started");
    write_log_entry(TEST_LOG_FILE, "DEBUG", "Processing file operations");
    write_log_entry(TEST_LOG_FILE, "INFO", "Demo completed");

    /* Create and process config file */
    const char * const config_content =
        "# Configuration file\\n"
        "[database]\\n"
        "host=localhost\\n"
        "port=5432\\n"
        "name=testdb\\n"
        "\\n"
        "[server]\\n"
        "port=8080\\n"
        "threads=4\\n";

    if (write_text_to_file(TEST_CONFIG_FILE, config_content) == 0) {
        process_config_file(TEST_CONFIG_FILE);
        remove(TEST_CONFIG_FILE);
    }

    /* Test binary file operations */
    demonstrate_binary_file_ops(TEST_BINARY_FILE);

    printf("\\n=== File operations demo completed ===\\n");
    printf("Note: Log file '%s' created for inspection\\n", TEST_LOG_FILE);

    return 0;
}