/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: signed_char_string_literal.c
 *
 * This case demonstrates a violation of STR00-C by using signed char
 * for string literals instead of plain char, which can cause type
 * compatibility issues and compiler warnings.
 */

#include <stdio.h>
#include <string.h>

int main(void) {
    /* VIOLATION: Using signed char for string literal */
    signed char signed_string[] = "Hello, World!";

    /* VIOLATION: This may cause warnings on some compilers */
    size_t len = strlen(signed_string);  /* Warning: incompatible pointer type */

    printf("String: %s\n", signed_string);  /* Warning: format mismatch */
    printf("Length: %zu\n", len);

    /* VIOLATION: Comparison with string function expecting char* */
    if (strcmp(signed_string, "Hello, World!") == 0) {  /* Warning */
        printf("Strings match\n");
    }

    /* VIOLATION: Character manipulation with wrong type */
    for (size_t i = 0; i < len; i++) {
        if (signed_string[i] >= 'A' && signed_string[i] <= 'Z') {
            signed_string[i] = signed_string[i] + 32;  /* Convert to lowercase */
        }
    }

    printf("Modified string: %s\n", signed_string);

    return 0;
}