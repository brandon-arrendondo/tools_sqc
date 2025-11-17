/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: log_operations_unchecked.c
 *
 * This case demonstrates violations where logging functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

/* Log level enumeration */
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3,
    LOG_CRITICAL = 4
} LogLevel;

/* Logger structure */
typedef struct {
    FILE *log_file;
    LogLevel min_level;
    char *log_format;
} Logger;

/* NON-COMPLIANT: No validation of logger initialization */
Logger *create_logger(const char *log_file_path, LogLevel min_level, const char *format) {
    Logger *logger = malloc(sizeof(Logger));

    /* No validation of log_file_path */
    logger->log_file = fopen(log_file_path, "a");  /* log_file_path could be NULL */

    logger->min_level = min_level;  /* No validation of min_level range */

    /* No validation of format */
    logger->log_format = malloc(strlen(format) + 1);  /* format could be NULL */
    strcpy(logger->log_format, format);

    return logger;
}

/* NON-COMPLIANT: No validation of log message parameters */
void log_message(Logger *logger, LogLevel level, const char *message) {
    /* No validation of logger or message */
    if (level >= logger->min_level) {  /* logger could be NULL */
        fprintf(logger->log_file, "%s\n", message);  /* message could be NULL */
        fflush(logger->log_file);
    }
}

/* NON-COMPLIANT: No validation of formatted log parameters */
void log_formatted(Logger *logger, LogLevel level, const char *format, ...) {
    /* No validation of logger or format */
    va_list args;
    va_start(args, format);

    char buffer[1024];  /* Fixed size without checking format result */
    vsnprintf(buffer, sizeof(buffer), format, args);  /* format could be NULL */

    log_message(logger, level, buffer);

    va_end(args);
}

/* NON-COMPLIANT: No validation of log rotation parameters */
void rotate_log_file(Logger *logger, size_t max_size, int max_files) {
    /* No validation of logger or parameters */
    fseek(logger->log_file, 0, SEEK_END);  /* logger could be NULL */
    long file_size = ftell(logger->log_file);

    if (file_size > (long)max_size) {  /* No validation of max_size */
        fclose(logger->log_file);

        /* Mock rotation without validation */
        for (int i = max_files - 1; i > 0; i--) {  /* max_files could be negative */
            char old_name[256], new_name[256];
            sprintf(old_name, "log.%d", i);
            sprintf(new_name, "log.%d", i + 1);
            rename(old_name, new_name);
        }

        rename("log", "log.1");
        logger->log_file = fopen("log", "w");
    }
}

/* NON-COMPLIANT: No validation of log filtering */
void set_log_filter(Logger *logger, const char *filter_pattern) {
    /* No validation of logger or filter_pattern */
    printf("Setting log filter: %s\n", filter_pattern);  /* filter_pattern could be NULL */

    /* Mock filter setting */
    char *pattern_copy = malloc(strlen(filter_pattern) + 1);  /* filter_pattern could be NULL */
    strcpy(pattern_copy, filter_pattern);
}

/* NON-COMPLIANT: No validation of structured logging */
void log_structured(Logger *logger, LogLevel level, const char *event_name,
                   const char *key_value_pairs[], size_t pair_count) {
    /* No validation of any parameters */
    fprintf(logger->log_file, "EVENT: %s ", event_name);  /* logger and event_name could be NULL */

    for (size_t i = 0; i < pair_count; i += 2) {  /* No validation of array bounds */
        fprintf(logger->log_file, "%s=%s ", key_value_pairs[i], key_value_pairs[i + 1]);  /* Array elements could be NULL */
    }

    fprintf(logger->log_file, "\n");
    fflush(logger->log_file);
}

/* NON-COMPLIANT: No validation of log aggregation */
void aggregate_logs(const char *input_pattern, const char *output_file, const char *time_window) {
    /* No validation of any parameters */
    FILE *output = fopen(output_file, "w");  /* output_file could be NULL */

    printf("Aggregating logs matching pattern: %s\n", input_pattern);  /* input_pattern could be NULL */
    printf("Time window: %s\n", time_window);  /* time_window could be NULL */

    /* Mock aggregation without validation */
    fprintf(output, "Aggregated log data\n");
    fclose(output);
}

/* NON-COMPLIANT: No validation of log searching */
void search_logs(Logger *logger, const char *search_term, time_t start_time, time_t end_time) {
    /* No validation of logger or search_term */
    printf("Searching logs for: %s\n", search_term);  /* search_term could be NULL */

    /* Mock search without validation */
    rewind(logger->log_file);  /* logger could be NULL */

    char line[1024];
    while (fgets(line, sizeof(line), logger->log_file)) {
        if (strstr(line, search_term)) {  /* search_term could be NULL */
            printf("Found: %s", line);
        }
    }
}

/* NON-COMPLIANT: No validation of log export */
void export_logs(Logger *logger, const char *export_format, const char *output_path,
                time_t start_time, time_t end_time) {
    /* No validation of any parameters */
    FILE *export_file = fopen(output_path, "w");  /* output_path could be NULL */

    printf("Exporting logs in format: %s\n", export_format);  /* export_format could be NULL */

    /* Mock export without validation */
    if (strcmp(export_format, "JSON") == 0) {  /* export_format could be NULL */
        fprintf(export_file, "{\n  \"logs\": []\n}\n");
    } else if (strcmp(export_format, "CSV") == 0) {
        fprintf(export_file, "timestamp,level,message\n");
    }

    fclose(export_file);
}

int main(void) {
    Logger *null_logger = NULL;
    char *null_string = NULL;
    const char **null_array = NULL;

    /* Examples of dangerous logging operations */
    // create_logger(null_string, -1, null_string);  /* NULL parameters and invalid level */
    // log_message(null_logger, LOG_INFO, null_string);  /* NULL parameters */
    // log_formatted(null_logger, LOG_ERROR, null_string, "arg1", "arg2");  /* NULL parameters */
    // rotate_log_file(null_logger, 0, -5);  /* NULL logger and invalid parameters */
    // set_log_filter(null_logger, null_string);  /* NULL parameters */
    // log_structured(null_logger, LOG_DEBUG, null_string, null_array, 100);  /* NULL parameters */
    // aggregate_logs(null_string, null_string, null_string);  /* NULL parameters */
    // search_logs(null_logger, null_string, 0, 0);  /* NULL parameters */
    // export_logs(null_logger, null_string, null_string, 0, 0);  /* NULL parameters */

    printf("Logging functions compiled but lack parameter validation\n");
    return 0;
}