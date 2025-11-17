/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: ctype_functions_misuse.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * character types with ctype.h functions, which expect int parameters
 * in the range of unsigned char or EOF.
 */

#include <stdio.h>
#include <ctype.h>

int main(void) {
    /* VIOLATION: Using signed char with ctype functions */
    signed char signed_chars[] = {-1, -50, 'A', 'b', '1', '@'};
    size_t count = sizeof(signed_chars) / sizeof(signed_chars[0]);

    printf("Testing signed char values with ctype functions:\n");
    for (size_t i = 0; i < count; i++) {
        signed char c = signed_chars[i];

        printf("Character: %d (0x%02X)\n", c, (unsigned char)c);

        /* VIOLATION: Passing negative values to ctype functions */
        /* This causes undefined behavior for negative values other than EOF */
        if (isalpha(c)) {  /* Undefined behavior for negative values */
            printf("  isalpha: true\n");
        }

        if (isdigit(c)) {  /* Undefined behavior for negative values */
            printf("  isdigit: true\n");
        }

        if (isprint(c)) {  /* Undefined behavior for negative values */
            printf("  isprint: true\n");
        }

        printf("\n");
    }

    /* VIOLATION: Direct use of char without proper conversion */
    char test_string[] = "Hello123!";
    printf("Analyzing string with improper ctype usage:\n");

    for (size_t i = 0; test_string[i] != '\0'; i++) {
        char c = test_string[i];

        /* On systems where char is signed and string contains high-bit characters,
         * this could cause undefined behavior */
        printf("'%c': ", c);

        if (isalpha(c)) {    /* Should cast to unsigned char first */
            printf("alpha ");
        }
        if (isdigit(c)) {    /* Should cast to unsigned char first */
            printf("digit ");
        }
        if (ispunct(c)) {    /* Should cast to unsigned char first */
            printf("punct ");
        }
        printf("\n");
    }

    /* VIOLATION: Extended ASCII characters with signed char */
    signed char extended_chars[] = {128, 150, 200, 255};  /* Negative on signed systems */

    printf("\nTesting extended ASCII with signed char:\n");
    for (size_t i = 0; i < 4; i++) {
        signed char c = extended_chars[i];

        printf("Value %d: ", c);

        /* VIOLATION: These calls have undefined behavior */
        if (isprint(c)) {
            printf("printable ");
        }
        if (isspace(c)) {
            printf("space ");
        }
        if (iscntrl(c)) {
            printf("control ");
        }
        printf("\n");
    }

    /* VIOLATION: Using char variables directly in ctype macros */
    char input;
    printf("\nEnter a character: ");
    input = getchar();

    /* Clear input buffer */
    while (getchar() != '\n');

    /* VIOLATION: Direct use without ensuring proper range */
    printf("Character analysis:\n");
    printf("isalnum: %d\n", isalnum(input));    /* Potential undefined behavior */
    printf("islower: %d\n", islower(input));    /* Potential undefined behavior */
    printf("isupper: %d\n", isupper(input));    /* Potential undefined behavior */

    /* VIOLATION: toupper/tolower with wrong type expectation */
    char result = toupper(input);  /* toupper returns int, not char */
    printf("Uppercase: %c\n", result);

    /* VIOLATION: Arithmetic with potentially negative char values */
    char char_diff = 'z' - 'a';
    printf("Character difference: %d\n", char_diff);

    /* Use in ctype function - could be negative */
    if (isprint(char_diff)) {  /* Undefined if char_diff is negative */
        printf("Difference is printable\n");
    }

    return 0;
}