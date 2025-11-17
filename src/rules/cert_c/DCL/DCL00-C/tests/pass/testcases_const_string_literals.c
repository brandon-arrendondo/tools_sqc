/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Pass Case: const_string_literals.c
 *
 * This case demonstrates compliant code that properly const-qualifies
 * string literals and string-related operations, preventing modification
 * attempts and ensuring type safety.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* COMPLIANT: Global const string literals */
static const char * const PROGRAM_NAME = "String Literals Demo";
static const char * const COPYRIGHT_NOTICE = "Copyright (c) 2024 Example Corp";
static const char * const LICENSE_TEXT = "Licensed under MIT License";

/* COMPLIANT: Const string arrays for lookup */
static const char * const WEEKDAY_NAMES[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};

static const char * const MONTH_ABBREVIATIONS[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

/* COMPLIANT: Const format strings */
static const char * const DATE_FORMAT = "%s %d, %Y";
static const char * const TIME_FORMAT = "%02d:%02d:%02d";
static const char * const LOG_ENTRY_FORMAT = "[%s] %s: %s";

/* COMPLIANT: Function with const string literal parameter */
size_t safe_string_length(const char *str) {
    /* COMPLIANT: Handle NULL string safely */
    if (!str) {
        return 0;
    }
    return strlen(str);
}

/* COMPLIANT: Function for safe string comparison */
int safe_string_compare(const char *str1, const char *str2) {
    /* COMPLIANT: Handle NULL strings safely */
    if (!str1 && !str2) {
        return 0;  /* Both NULL, considered equal */
    }
    if (!str1) {
        return -1;  /* str1 is NULL, str2 is not */
    }
    if (!str2) {
        return 1;   /* str2 is NULL, str1 is not */
    }

    return strcmp(str1, str2);
}

/* COMPLIANT: Function demonstrating proper string literal usage */
void demonstrate_string_literals(void) {
    /* COMPLIANT: Local const string literals */
    const char * const greeting = "Hello, World!";
    const char * const message = "This is a demonstration of const string literals";
    const char * const empty_string = "";

    printf("String Literals Demonstration:\\n");
    printf("  Greeting: %s\\n", greeting);
    printf("  Message: %s\\n", message);
    printf("  Empty string length: %zu\\n", safe_string_length(empty_string));

    /* COMPLIANT: Using const string literals in operations */
    const size_t greeting_length = safe_string_length(greeting);
    const size_t message_length = safe_string_length(message);

    printf("  Greeting length: %zu characters\\n", greeting_length);
    printf("  Message length: %zu characters\\n", message_length);

    /* COMPLIANT: String comparison with const literals */
    const char * const test_string = "Hello, World!";
    const int comparison_result = safe_string_compare(greeting, test_string);

    printf("  String comparison result: %d (%s)\\n",
           comparison_result,
           comparison_result == 0 ? "equal" : "not equal");
}

/* COMPLIANT: Function processing const string arrays */
void display_weekdays(void) {
    /* COMPLIANT: Local const for array processing */
    const size_t weekday_count = sizeof(WEEKDAY_NAMES) / sizeof(WEEKDAY_NAMES[0]);
    const char * const section_header = "Weekdays:";

    printf("\\n%s\\n", section_header);

    for (size_t i = 0; i < weekday_count; i++) {
        const char * const current_day = WEEKDAY_NAMES[i];
        const size_t day_length = safe_string_length(current_day);

        printf("  %zu. %-10s (length: %zu)\\n", i + 1, current_day, day_length);
    }

    /* COMPLIANT: Find longest weekday name using const data */
    const char * longest_day = WEEKDAY_NAMES[0];
    size_t max_length = safe_string_length(longest_day);

    for (size_t i = 1; i < weekday_count; i++) {
        const size_t current_length = safe_string_length(WEEKDAY_NAMES[i]);
        if (current_length > max_length) {
            max_length = current_length;
            longest_day = WEEKDAY_NAMES[i];
        }
    }

    printf("  Longest weekday name: '%s' (%zu characters)\\n", longest_day, max_length);
}

/* COMPLIANT: Function for string formatting with const templates */
char *format_date_string(const int day, const int month, const int year) {
    /* COMPLIANT: Validate parameters */
    if (month < 1 || month > 12 || day < 1 || day > 31 || year < 1900) {
        return NULL;
    }

    /* COMPLIANT: Local const for month validation */
    const size_t month_count = sizeof(MONTH_ABBREVIATIONS) / sizeof(MONTH_ABBREVIATIONS[0]);

    if ((size_t)month > month_count) {
        return NULL;
    }

    /* COMPLIANT: Use const month abbreviation */
    const char * const month_abbrev = MONTH_ABBREVIATIONS[month - 1];

    /* COMPLIANT: Calculate buffer size needed */
    const size_t max_date_length = 20;  /* "Dec 31, 2024" + null terminator + margin */
    char *date_buffer = malloc(max_date_length);

    if (!date_buffer) {
        return NULL;
    }

    /* COMPLIANT: Format using const format string */
    const int result = snprintf(date_buffer, max_date_length, DATE_FORMAT,
                               month_abbrev, day, year);

    if (result < 0 || (size_t)result >= max_date_length) {
        free(date_buffer);
        return NULL;
    }

    return date_buffer;
}

/* COMPLIANT: Function for time formatting */
char *format_time_string(const int hours, const int minutes, const int seconds) {
    /* COMPLIANT: Validate time parameters */
    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59 ||
        seconds < 0 || seconds > 59) {
        return NULL;
    }

    /* COMPLIANT: Allocate buffer for time string */
    const size_t time_buffer_size = 10;  /* "HH:MM:SS" + null terminator */
    char *time_buffer = malloc(time_buffer_size);

    if (!time_buffer) {
        return NULL;
    }

    /* COMPLIANT: Format using const format string */
    const int result = snprintf(time_buffer, time_buffer_size, TIME_FORMAT,
                               hours, minutes, seconds);

    if (result < 0 || (size_t)result >= time_buffer_size) {
        free(time_buffer);
        return NULL;
    }

    return time_buffer;
}

/* COMPLIANT: Function for log entry creation */
void create_log_entry(const char *timestamp, const char *level, const char *message) {
    /* COMPLIANT: Validate parameters */
    if (!timestamp || !level || !message) {
        printf("Error: NULL parameter in log entry\\n");
        return;
    }

    /* COMPLIANT: Calculate required buffer size */
    const size_t timestamp_len = safe_string_length(timestamp);
    const size_t level_len = safe_string_length(level);
    const size_t message_len = safe_string_length(message);
    const size_t format_overhead = 10;  /* For "[", "] ", ": ", and null terminator */
    const size_t total_size = timestamp_len + level_len + message_len + format_overhead;

    char *log_buffer = malloc(total_size);
    if (!log_buffer) {
        printf("Error: Cannot allocate memory for log entry\\n");
        return;
    }

    /* COMPLIANT: Format using const format string */
    const int result = snprintf(log_buffer, total_size, LOG_ENTRY_FORMAT,
                               timestamp, level, message);

    if (result > 0 && (size_t)result < total_size) {
        printf("Log entry: %s\\n", log_buffer);
    } else {
        printf("Error: Log entry formatting failed\\n");
    }

    free(log_buffer);
}

/* COMPLIANT: Function demonstrating string searching in const data */
void demonstrate_string_search(void) {
    /* COMPLIANT: Const search targets */
    const char * const search_text = "The quick brown fox jumps over the lazy dog";
    const char * const search_terms[] = {"quick", "fox", "lazy", "cat", "dog"};
    const size_t term_count = sizeof(search_terms) / sizeof(search_terms[0]);

    printf("\\nString Search Demonstration:\\n");
    printf("  Search text: %s\\n", search_text);

    for (size_t i = 0; i < term_count; i++) {
        const char * const current_term = search_terms[i];
        const char * const found_position = strstr(search_text, current_term);

        if (found_position) {
            const size_t position = (size_t)(found_position - search_text);
            printf("  '%s' found at position %zu\\n", current_term, position);
        } else {
            printf("  '%s' not found\\n", current_term);
        }
    }
}

/* COMPLIANT: Function for character analysis in const strings */
void analyze_string_characters(const char *text, const char *description) {
    if (!text || !description) {
        return;
    }

    printf("\\nCharacter Analysis: %s\\n", description);
    printf("  Text: %s\\n", text);

    /* COMPLIANT: Character counting using const data */
    const size_t text_length = safe_string_length(text);
    size_t vowel_count = 0;
    size_t consonant_count = 0;
    size_t digit_count = 0;
    size_t space_count = 0;
    size_t other_count = 0;

    /* COMPLIANT: Const vowel lookup string */
    const char * const vowels = "aeiouAEIOU";

    for (size_t i = 0; i < text_length; i++) {
        const char current_char = text[i];

        if (current_char == ' ') {
            space_count++;
        } else if (current_char >= '0' && current_char <= '9') {
            digit_count++;
        } else if (strchr(vowels, current_char)) {
            vowel_count++;
        } else if ((current_char >= 'A' && current_char <= 'Z') ||
                   (current_char >= 'a' && current_char <= 'z')) {
            consonant_count++;
        } else {
            other_count++;
        }
    }

    printf("  Total characters: %zu\\n", text_length);
    printf("  Vowels: %zu\\n", vowel_count);
    printf("  Consonants: %zu\\n", consonant_count);
    printf("  Digits: %zu\\n", digit_count);
    printf("  Spaces: %zu\\n", space_count);
    printf("  Other: %zu\\n", other_count);
}

int main(void) {
    printf("=== %s ===\\n", PROGRAM_NAME);
    printf("%s\\n", COPYRIGHT_NOTICE);
    printf("%s\\n\\n", LICENSE_TEXT);

    /* Demonstrate basic string literal operations */
    demonstrate_string_literals();

    /* Display weekday information */
    display_weekdays();

    /* Test date and time formatting */
    char *formatted_date = format_date_string(25, 12, 2024);
    if (formatted_date) {
        printf("\\nFormatted date: %s\\n", formatted_date);
        free(formatted_date);
    }

    char *formatted_time = format_time_string(14, 30, 45);
    if (formatted_time) {
        printf("Formatted time: %s\\n", formatted_time);
        free(formatted_time);
    }

    /* Create sample log entries */
    printf("\\nLog Entry Examples:\\n");
    create_log_entry("2024-01-01 10:00:00", "INFO", "Application started");
    create_log_entry("2024-01-01 10:00:01", "DEBUG", "Processing string literals");
    create_log_entry("2024-01-01 10:00:02", "ERROR", "Sample error message");

    /* Demonstrate string searching */
    demonstrate_string_search();

    /* Analyze character content */
    const char * const sample_text = "Hello World! This is a test string with 123 numbers.";
    analyze_string_characters(sample_text, "Sample Text Analysis");

    printf("\\n=== String literals demonstration completed ===\\n");

    return 0;
}