/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: unvalidated_string_operations.c
 *
 * This case demonstrates violations where string operation functions
 * don't validate their parameters for safety.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* NON-COMPLIANT: No validation of string parameter */
void to_uppercase(char *str) {
    /* Direct manipulation without NULL check or length validation */
    while (*str) {
        *str = toupper(*str);  /* str could be NULL */
        str++;
    }
}

/* NON-COMPLIANT: No validation of destination buffer size */
void string_concat(char *dest, const char *src1, const char *src2) {
    /* Concatenating without checking buffer capacity */
    strcpy(dest, src1);  /* No check if dest is large enough */
    strcat(dest, src2);  /* Could overflow dest buffer */
}

/* NON-COMPLIANT: No validation of format string */
void formatted_print(const char *format, const char *data) {
    /* Using format string without validation */
    printf(format, data);  /* format could contain unexpected specifiers */
}

/* NON-COMPLIANT: No validation of substring boundaries */
char *extract_substring(const char *str, size_t start, size_t length) {
    static char buffer[256];
    /* Extracting without bounds checking */
    strncpy(buffer, str + start, length);  /* start could exceed string length */
    buffer[length] = '\0';
    return buffer;
}

/* NON-COMPLIANT: No validation of delimiter presence */
void split_string(char *str, char delimiter) {
    /* Splitting without checking if delimiter exists */
    char *pos = strchr(str, delimiter);
    *pos = '\0';  /* pos could be NULL if delimiter not found */
}

/* NON-COMPLIANT: No validation of replacement parameters */
void replace_char(char *str, char old_char, char new_char) {
    /* Replacing without NULL check or validation */
    while (*str) {
        if (*str == old_char) {
            *str = new_char;  /* str could be NULL or read-only */
        }
        str++;
    }
}

/* NON-COMPLIANT: No validation of trim boundaries */
void trim_string(char *str, size_t trim_left, size_t trim_right) {
    size_t len = strlen(str);
    /* Trimming without validating parameters */
    memmove(str, str + trim_left, len - trim_left - trim_right);
    str[len - trim_left - trim_right] = '\0';  /* Could underflow */
}

int main(void) {
    char buffer[20] = "Hello";
    char *null_string = NULL;

    /* Examples of dangerous operations */
    // to_uppercase(null_string);  /* NULL pointer access */
    // string_concat(buffer, "Very long string", "Another long string");  /* Buffer overflow */
    // formatted_print("%s %d %f", "Only one argument");  /* Format mismatch */
    // extract_substring("Short", 100, 50);  /* Out of bounds */
    // split_string("No delimiter here", ',');  /* NULL pointer dereference */
    // replace_char(null_string, 'a', 'b');  /* NULL pointer access */
    // trim_string("Hi", 5, 5);  /* Invalid trim values */

    printf("String functions compiled but lack parameter validation\n");
    return 0;
}