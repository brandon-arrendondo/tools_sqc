/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: unsigned_char_string_literal.c
 *
 * This case demonstrates a violation of STR00-C by using unsigned char
 * for string literals instead of plain char, which can cause type
 * compatibility issues with standard string functions.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(void) {
    /* VIOLATION: Using unsigned char for string literal */
    unsigned char unsigned_string[] = "Welcome to C programming!";

    /* VIOLATION: Type mismatch with standard string functions */
    size_t len = strlen(unsigned_string);  /* Warning: incompatible pointer type */

    printf("String: %s\n", unsigned_string);  /* Warning: format specifier */
    printf("Length: %zu\n", len);

    /* VIOLATION: Using with string library functions */
    unsigned char *found = strstr(unsigned_string, "C");  /* Warning */
    if (found) {
        printf("Found 'C' at position: %ld\n", found - unsigned_string);
    }

    /* VIOLATION: Character classification with wrong type */
    for (size_t i = 0; i < len; i++) {
        if (isalpha(unsigned_string[i])) {  /* Potential issue */
            unsigned_string[i] = toupper(unsigned_string[i]);
        }
    }

    /* VIOLATION: String concatenation with type mismatch */
    unsigned char suffix[] = " - Modified";
    strcat(unsigned_string, suffix);  /* Warning: incompatible types */

    printf("Final string: %s\n", unsigned_string);

    return 0;
}