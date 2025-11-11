/*
 * Rule: STR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR00-C violation
 */

/*
 * CERT C STR00-C Pass Case: proper_string_operations.c
 *
 * This case demonstrates compliant code that uses appropriate character
 * types for common string operations, maintaining type consistency
 * and avoiding compilation warnings.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
    printf("Proper string operations with consistent character types:\n\n");

    /* COMPLIANT: String initialization and basic operations */
    char source[] = "Hello, World!";
    char destination[100];
    char buffer[200];

    printf("Basic string operations:\n");
    printf("Source: %s\n", source);

    /* COMPLIANT: String copying */
    strcpy(destination, source);
    printf("Copied: %s\n", destination);

    /* COMPLIANT: String concatenation */
    strcpy(buffer, "Greeting: ");
    strcat(buffer, source);
    printf("Concatenated: %s\n", buffer);

    /* COMPLIANT: String length operations */
    size_t source_len = strlen(source);
    size_t buffer_len = strlen(buffer);

    printf("Source length: %zu\n", source_len);
    printf("Buffer length: %zu\n", buffer_len);

    /* COMPLIANT: String comparison */
    char compare1[] = "Apple";
    char compare2[] = "Banana";
    char compare3[] = "Apple";

    printf("\nString comparison:\n");
    printf("'%s' vs '%s': %d\n", compare1, compare2, strcmp(compare1, compare2));
    printf("'%s' vs '%s': %d\n", compare1, compare3, strcmp(compare1, compare3));

    /* COMPLIANT: Case-insensitive comparison (manual implementation) */
    char upper1[] = "HELLO";
    char lower1[] = "hello";

    printf("Case-insensitive comparison:\n");

    /* Convert both to lowercase for comparison */
    char temp1[strlen(upper1) + 1];
    char temp2[strlen(lower1) + 1];

    strcpy(temp1, upper1);
    strcpy(temp2, lower1);

    /* Convert to lowercase */
    for (size_t i = 0; temp1[i] != '\0'; i++) {
        if (temp1[i] >= 'A' && temp1[i] <= 'Z') {
            temp1[i] = temp1[i] + 32;
        }
    }

    int case_insensitive_result = strcmp(temp1, temp2);
    printf("'%s' vs '%s' (case-insensitive): %d\n", upper1, lower1, case_insensitive_result);

    /* COMPLIANT: String searching */
    char search_text[] = "The quick brown fox jumps over the lazy dog";
    char *found_position;

    printf("\nString searching:\n");
    printf("Text: %s\n", search_text);

    /* Search for character */
    found_position = strchr(search_text, 'q');
    if (found_position != NULL) {
        printf("Found 'q' at position: %ld\n", found_position - search_text);
    }

    /* Search for substring */
    found_position = strstr(search_text, "fox");
    if (found_position != NULL) {
        printf("Found 'fox' at position: %ld\n", found_position - search_text);
    }

    /* Search for last occurrence */
    found_position = strrchr(search_text, 'o');
    if (found_position != NULL) {
        printf("Last 'o' at position: %ld\n", found_position - search_text);
    }

    /* COMPLIANT: String tokenization */
    char tokenize_data[] = "apple,banana,cherry,date,elderberry";
    char *token;
    char *context;  /* For strtok_r if available */

    printf("\nString tokenization:\n");
    printf("Data: %s\n", tokenize_data);
    printf("Tokens: ");

    /* Use strtok (modifies original string) */
    token = strtok(tokenize_data, ",");
    while (token != NULL) {
        printf("'%s' ", token);
        token = strtok(NULL, ",");
    }
    printf("\n");

    /* COMPLIANT: String manipulation and formatting */
    char format_buffer[300];
    const char *name = "Alice";
    int age = 25;
    double salary = 75000.50;

    printf("\nString formatting:\n");

    /* Use snprintf for safe formatting */
    int result = snprintf(format_buffer, sizeof(format_buffer),
                         "Employee: %s, Age: %d, Salary: $%.2f",
                         name, age, salary);

    if (result > 0 && (size_t)result < sizeof(format_buffer)) {
        printf("Formatted: %s\n", format_buffer);
    } else {
        printf("Formatting error or buffer too small\n");
    }

    /* COMPLIANT: String building with proper bounds checking */
    char build_buffer[150];
    const char *parts[] = {"Part1", "Part2", "Part3", "Part4"};
    const char *separator = " - ";

    printf("\nString building:\n");

    strcpy(build_buffer, "Result: ");

    for (int i = 0; i < 4; i++) {
        if (strlen(build_buffer) + strlen(parts[i]) + strlen(separator) + 1 < sizeof(build_buffer)) {
            if (i > 0) {
                strcat(build_buffer, separator);
            }
            strcat(build_buffer, parts[i]);
        } else {
            printf("Buffer would overflow, stopping\n");
            break;
        }
    }

    printf("Built string: %s\n", build_buffer);

    /* COMPLIANT: String validation and sanitization */
    char input_data[] = "Valid input with some numbers 123 and symbols !@#";
    char sanitized[sizeof(input_data)];

    printf("\nString sanitization:\n");
    printf("Original: %s\n", input_data);

    /* Copy only alphanumeric characters and spaces */
    size_t dest_index = 0;
    for (size_t i = 0; input_data[i] != '\0' && dest_index < sizeof(sanitized) - 1; i++) {
        char c = input_data[i];
        if ((c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            c == ' ') {
            sanitized[dest_index++] = c;
        }
    }
    sanitized[dest_index] = '\0';

    printf("Sanitized: %s\n", sanitized);

    /* COMPLIANT: Dynamic string operations */
    printf("\nDynamic string operations:\n");

    const char *strings_to_combine[] = {
        "Dynamic", "string", "combination", "example"
    };
    int string_count = 4;

    /* Calculate total length needed */
    size_t total_length = 1;  /* For null terminator */
    for (int i = 0; i < string_count; i++) {
        total_length += strlen(strings_to_combine[i]);
        if (i < string_count - 1) {
            total_length += 1;  /* For space separator */
        }
    }

    /* Allocate memory */
    char *dynamic_string = malloc(total_length);
    if (dynamic_string != NULL) {
        strcpy(dynamic_string, "");

        /* Combine strings */
        for (int i = 0; i < string_count; i++) {
            strcat(dynamic_string, strings_to_combine[i]);
            if (i < string_count - 1) {
                strcat(dynamic_string, " ");
            }
        }

        printf("Dynamic string: %s\n", dynamic_string);
        printf("Total length: %zu\n", strlen(dynamic_string));

        free(dynamic_string);
    }

    /* COMPLIANT: String duplication and management */
    printf("\nString duplication:\n");

    const char *original = "String to duplicate";
    char *duplicate = malloc(strlen(original) + 1);

    if (duplicate != NULL) {
        strcpy(duplicate, original);
        printf("Original: %s\n", original);
        printf("Duplicate: %s\n", duplicate);

        /* Modify duplicate */
        for (size_t i = 0; duplicate[i] != '\0'; i++) {
            if (duplicate[i] >= 'a' && duplicate[i] <= 'z') {
                duplicate[i] = duplicate[i] - 32;  /* Convert to uppercase */
            }
        }

        printf("Modified duplicate: %s\n", duplicate);

        free(duplicate);
    }

    /* COMPLIANT: String reversal */
    char reverse_text[] = "Reverse this text";
    size_t reverse_len = strlen(reverse_text);

    printf("\nString reversal:\n");
    printf("Original: %s\n", reverse_text);

    /* Reverse in place */
    for (size_t i = 0; i < reverse_len / 2; i++) {
        char temp = reverse_text[i];
        reverse_text[i] = reverse_text[reverse_len - 1 - i];
        reverse_text[reverse_len - 1 - i] = temp;
    }

    printf("Reversed: %s\n", reverse_text);

    /* COMPLIANT: Path manipulation */
    printf("\nPath manipulation:\n");

    char file_path[] = "/home/user/documents/readme.txt";
    printf("Full path: %s\n", file_path);

    /* Extract directory */
    char *last_slash = strrchr(file_path, '/');
    if (last_slash != NULL) {
        char directory[last_slash - file_path + 1];
        strncpy(directory, file_path, last_slash - file_path);
        directory[last_slash - file_path] = '\0';

        printf("Directory: %s\n", directory);

        /* Extract filename */
        char *filename = last_slash + 1;
        printf("Filename: %s\n", filename);

        /* Extract extension */
        char *last_dot = strrchr(filename, '.');
        if (last_dot != NULL) {
            printf("Extension: %s\n", last_dot);
        }
    }

    return 0;
}