/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: character_classification_loops.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * character types in loops and character classification operations,
 * leading to sign-dependent behavior and potential infinite loops.
 */

#include <stdio.h>
#include <ctype.h>

int main(void) {
    /* VIOLATION: Using signed char in character loop */
    printf("Testing character loops with signed char:\n");

    /* VIOLATION: Loop with signed char may behave unexpectedly */
    for (signed char c = 'A'; c <= 'Z'; c++) {
        printf("%c ", c);
        if (c == 'E') {
            printf("(stopping early to prevent issues) ");
            break;
        }
    }
    printf("\n");

    /* VIOLATION: Dangerous loop with potential wrap-around */
    printf("\nDangerous reverse loop:\n");
    for (signed char c = 'z'; c >= 'a'; c--) {  /* May wrap to positive values */
        printf("%c ", c);
        if (c == 'v') {  /* Safety break */
            printf("(stopping to prevent infinite loop) ");
            break;
        }
    }
    printf("\n");

    /* VIOLATION: Character range checking with sign issues */
    printf("\nCharacter range testing:\n");
    signed char test_chars[] = {65, 90, 97, 122, 128, 150, 200, 255, -1, -50};
    size_t num_chars = sizeof(test_chars) / sizeof(test_chars[0]);

    for (size_t i = 0; i < num_chars; i++) {
        signed char c = test_chars[i];
        printf("Character %zu (value: %d):\n", i, c);

        /* VIOLATION: Character classification with potentially negative values */
        if (isalpha(c)) {  /* Undefined behavior for negative values */
            printf("  Is alphabetic\n");
        }

        if (isdigit(c)) {  /* Undefined behavior for negative values */
            printf("  Is digit\n");
        }

        if (isprint(c)) {  /* Undefined behavior for negative values */
            printf("  Is printable\n");
        }

        /* VIOLATION: Range checks with sign-dependent behavior */
        if (c >= 'A' && c <= 'Z') {
            printf("  In uppercase range\n");
        }

        if (c >= 'a' && c <= 'z') {
            printf("  In lowercase range\n");
        }

        if (c >= '0' && c <= '9') {
            printf("  In digit range\n");
        }
    }

    /* VIOLATION: ASCII table iteration with wrong type */
    printf("\nASCII table subset (using char):\n");
    for (char ascii = 32; ascii < 127; ascii++) {  /* Sign-dependent on some systems */
        if (ascii % 16 == 0) {
            printf("\n%3d: ", ascii);
        }
        if (isprint(ascii)) {  /* Potential undefined behavior */
            printf("%c ", ascii);
        } else {
            printf(". ");
        }

        /* Safety break to prevent issues */
        if (ascii == 96) {
            printf("... (truncated)");
            break;
        }
    }
    printf("\n");

    /* VIOLATION: Character frequency counting with sign issues */
    printf("\nCharacter frequency analysis:\n");
    const char *text = "Hello World! This contains various characters: 123 @#$";
    int frequency[256] = {0};  /* Array size assumes unsigned char range */

    for (size_t i = 0; text[i] != '\0'; i++) {
        char c = text[i];

        /* VIOLATION: Using char as array index */
        if (c >= 0) {  /* Check needed due to potential negative values */
            frequency[c]++;
        } else {
            printf("Negative character value encountered: %d\n", c);
        }
    }

    /* Display frequency of printable characters */
    for (int i = 32; i < 127; i++) {
        if (frequency[i] > 0) {
            printf("'%c': %d times\n", i, frequency[i]);
        }
    }

    /* VIOLATION: Case conversion loop with sign issues */
    printf("\nCase conversion with character types:\n");
    signed char mixed_case[] = "MiXeD CaSe StRiNg";

    for (size_t i = 0; mixed_case[i] != '\0'; i++) {
        signed char c = mixed_case[i];

        /* VIOLATION: toupper/tolower with signed char */
        if (islower(c)) {  /* Potential undefined behavior */
            mixed_case[i] = toupper(c);  /* Returns int, assigning to signed char */
        } else if (isupper(c)) {  /* Potential undefined behavior */
            mixed_case[i] = tolower(c);  /* Returns int, assigning to signed char */
        }
    }

    printf("Converted string: %s\n", mixed_case);  /* Warning */

    /* VIOLATION: Character validation loop */
    printf("\nCharacter validation with wrong types:\n");
    unsigned char input[] = "Input123!@#";

    int alpha_count = 0, digit_count = 0, punct_count = 0;

    for (size_t i = 0; input[i] != '\0'; i++) {
        unsigned char c = input[i];

        /* VIOLATION: Passing unsigned char to ctype functions */
        /* While this might work, it's inconsistent with the expectation of unsigned char */
        if (isalpha(c)) {
            alpha_count++;
        } else if (isdigit(c)) {
            digit_count++;
        } else if (ispunct(c)) {
            punct_count++;
        }
    }

    printf("Alphabetic: %d, Digits: %d, Punctuation: %d\n",
           alpha_count, digit_count, punct_count);

    /* VIOLATION: Character enumeration with wrap-around risk */
    printf("\nCharacter enumeration risk:\n");
    char start_char = 'A';
    char end_char = 'Z';

    printf("Characters from %c to %c: ", start_char, end_char);
    for (char c = start_char; c <= end_char; c++) {
        printf("%c", c);
        /* This loop is generally safe for ASCII, but demonstrates the pattern */
        if (c == 'J') {  /* Early termination for demo */
            printf("...(truncated)");
            break;
        }
    }
    printf("\n");

    /* VIOLATION: Boundary testing with character types */
    printf("\nBoundary testing:\n");
    signed char boundary_chars[] = {0, 127, -128, -1};
    unsigned char uboundary_chars[] = {0, 127, 128, 255};

    printf("Signed char boundaries:\n");
    for (size_t i = 0; i < 4; i++) {
        signed char c = boundary_chars[i];
        printf("Value %d: isprint=%d, isalpha=%d\n",
               c, isprint(c), isalpha(c));  /* Potential undefined behavior */
    }

    printf("Unsigned char boundaries:\n");
    for (size_t i = 0; i < 4; i++) {
        unsigned char c = uboundary_chars[i];
        printf("Value %d: isprint=%d, isalpha=%d\n",
               c, isprint(c), isalpha(c));
    }

    return 0;
}