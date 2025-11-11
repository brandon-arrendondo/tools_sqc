/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Pass Case: safe_time_functions.c
 *
 * This case demonstrates compliant usage of asctime() and ctime()
 * by properly handling return values without modification.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* COMPLIANT: Safe immediate use of time functions */
void safe_immediate_time_display(void) {
    time_t current_time = time(NULL);

    /* Safe immediate use without storing or modifying */
    printf("Current time (ctime): %s", ctime(&current_time));

    struct tm *time_info = localtime(&current_time);
    if (time_info != NULL) {
        printf("Current time (asctime): %s", asctime(time_info));
    }
}

/* COMPLIANT: Safe time string copying */
void safe_time_string_copy(void) {
    time_t current_time = time(NULL);
    const char *time_string = ctime(&current_time);

    if (time_string != NULL) {
        /* Create a copy for safe modification */
        char *time_copy = malloc(strlen(time_string) + 1);

        if (time_copy != NULL) {
            strcpy(time_copy, time_string);

            /* Safe to modify the copy - remove newline */
            size_t len = strlen(time_copy);
            if (len > 0 && time_copy[len - 1] == '\n') {
                time_copy[len - 1] = '\0';
            }

            printf("Time without newline: %s\n", time_copy);
            free(time_copy);
        }
    }
}

/* COMPLIANT: Safe time formatting with strftime */
void safe_time_formatting(void) {
    time_t current_time = time(NULL);
    struct tm *time_info = localtime(&current_time);

    if (time_info != NULL) {
        /* Use strftime for safe custom formatting */
        char formatted_time[100];

        strftime(formatted_time, sizeof(formatted_time),
                "%Y-%m-%d %H:%M:%S", time_info);
        printf("Formatted time: %s\n", formatted_time);

        strftime(formatted_time, sizeof(formatted_time),
                "%A, %B %d, %Y", time_info);
        printf("Readable date: %s\n", formatted_time);
    }
}

/* COMPLIANT: Safe time string processing */
void safe_time_string_processing(void) {
    time_t current_time = time(NULL);
    struct tm *time_info = gmtime(&current_time);

    if (time_info != NULL) {
        const char *time_string = asctime(time_info);

        if (time_string != NULL) {
            /* Process time string safely using new buffer */
            size_t time_len = strlen(time_string);
            char *processed_time = malloc(time_len + 20);

            if (processed_time != NULL) {
                sprintf(processed_time, "UTC: %s", time_string);

                /* Remove newline from our copy */
                char *newline = strchr(processed_time, '\n');
                if (newline != NULL) {
                    *newline = '\0';
                }

                printf("Processed time: %s\n", processed_time);
                free(processed_time);
            }
        }
    }
}

/* COMPLIANT: Safe time comparison */
void safe_time_comparison(void) {
    time_t time1 = time(NULL);

    /* Wait a moment */
    for (volatile int i = 0; i < 1000000; i++);

    time_t time2 = time(NULL);

    /* Use time strings immediately for comparison */
    const char *str1 = ctime(&time1);
    const char *str2 = ctime(&time2);

    if (str1 != NULL && str2 != NULL) {
        printf("Time 1: %s", str1);
        printf("Time 2: %s", str2);

        if (strcmp(str1, str2) == 0) {
            printf("Times are identical\n");
        } else {
            printf("Times are different\n");
        }
    }
}

/* COMPLIANT: Safe time logging function */
void safe_time_log(const char *message) {
    time_t now = time(NULL);
    const char *time_str = ctime(&now);

    /* Create log entry in new buffer */
    if (time_str != NULL && message != NULL) {
        size_t log_size = strlen(time_str) + strlen(message) + 20;
        char *log_entry = malloc(log_size);

        if (log_entry != NULL) {
            /* Build log entry, handling newline properly */
            snprintf(log_entry, log_size, "[%.*s] %s",
                    (int)(strlen(time_str) - 1), time_str, message);
            printf("LOG: %s\n", log_entry);
            free(log_entry);
        }
    }
}

/* COMPLIANT: Safe time zone handling */
void safe_timezone_handling(void) {
    time_t current_time = time(NULL);

    /* Get local time */
    struct tm *local_time = localtime(&current_time);
    if (local_time != NULL) {
        char local_buffer[100];
        strftime(local_buffer, sizeof(local_buffer),
                "%Y-%m-%d %H:%M:%S %Z", local_time);
        printf("Local time: %s\n", local_buffer);
    }

    /* Get UTC time */
    struct tm *utc_time = gmtime(&current_time);
    if (utc_time != NULL) {
        char utc_buffer[100];
        strftime(utc_buffer, sizeof(utc_buffer),
                "%Y-%m-%d %H:%M:%S UTC", utc_time);
        printf("UTC time: %s\n", utc_buffer);
    }
}

/* COMPLIANT: Safe time arithmetic */
void safe_time_arithmetic(void) {
    time_t current_time = time(NULL);

    /* Calculate future and past times */
    time_t future_time = current_time + 3600;  /* +1 hour */
    time_t past_time = current_time - 3600;    /* -1 hour */

    /* Display times safely */
    printf("Past time (-1h): %s", ctime(&past_time));
    printf("Current time: %s", ctime(&current_time));
    printf("Future time (+1h): %s", ctime(&future_time));
}

/* COMPLIANT: Safe time validation */
int safe_validate_time_string(const char *expected_format) {
    time_t current_time = time(NULL);
    const char *time_str = ctime(&current_time);

    if (time_str == NULL || expected_format == NULL) {
        return 0;
    }

    /* Validate format without modifying original string */
    /* This is a simple length check - more sophisticated validation possible */
    size_t expected_len = strlen(expected_format);
    size_t actual_len = strlen(time_str);

    /* ctime format is usually "Day Mon DD HH:MM:SS YYYY\n" (25 chars) */
    if (actual_len == 25 && time_str[actual_len - 1] == '\n') {
        printf("Time string format is valid\n");
        return 1;
    } else {
        printf("Time string format is unexpected\n");
        return 0;
    }
}

/* COMPLIANT: Safe time parsing simulation */
void safe_time_parsing_demo(void) {
    time_t current_time = time(NULL);
    struct tm *time_info = localtime(&current_time);

    if (time_info != NULL) {
        const char *time_str = asctime(time_info);

        if (time_str != NULL) {
            /* Extract components safely using new buffer */
            char *parse_buffer = strdup(time_str);

            if (parse_buffer != NULL) {
                printf("Original: %s", time_str);

                /* Parse components from copy */
                char *day = strtok(parse_buffer, " ");
                char *month = strtok(NULL, " ");
                char *date = strtok(NULL, " ");
                char *time_part = strtok(NULL, " ");
                char *year = strtok(NULL, " \n");

                printf("Parsed components:\n");
                printf("  Day: %s\n", day ?: "N/A");
                printf("  Month: %s\n", month ?: "N/A");
                printf("  Date: %s\n", date ?: "N/A");
                printf("  Time: %s\n", time_part ?: "N/A");
                printf("  Year: %s\n", year ?: "N/A");

                free(parse_buffer);
            }
        }
    }
}

int main(void) {
    printf("=== ENV30-C Safe Time Functions Usage Demo ===\n");

    printf("\n1. Safe immediate time display:\n");
    safe_immediate_time_display();

    printf("\n2. Safe time string copy:\n");
    safe_time_string_copy();

    printf("\n3. Safe time formatting:\n");
    safe_time_formatting();

    printf("\n4. Safe time string processing:\n");
    safe_time_string_processing();

    printf("\n5. Safe time comparison:\n");
    safe_time_comparison();

    printf("\n6. Safe time logging:\n");
    safe_time_log("Application started");

    printf("\n7. Safe timezone handling:\n");
    safe_timezone_handling();

    printf("\n8. Safe time arithmetic:\n");
    safe_time_arithmetic();

    printf("\n9. Safe time validation:\n");
    safe_validate_time_string("standard_ctime_format");

    printf("\n10. Safe time parsing demo:\n");
    safe_time_parsing_demo();

    return 0;
}