/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: character_constants_wrong_type.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * types for character constants and mixing character constant types
 * inconsistently throughout the program.
 */

#include <stdio.h>
#include <wchar.h>

int main(void) {
    /* VIOLATION: Using signed char for character constants */
    signed char signed_constants[] = {'A', 'B', 'C', '\n'};

    /* VIOLATION: Using unsigned char for character constants */
    unsigned char unsigned_constants[] = {'X', 'Y', 'Z', '\0'};

    printf("Character constant type mismatches:\n");

    /* VIOLATION: Mixing different char types with constants */
    for (size_t i = 0; i < 4; i++) {
        if (signed_constants[i] == 'A') {      /* Comparison warning */
            printf("Found 'A' in signed array\n");
        }

        if (unsigned_constants[i] == 'X') {    /* Comparison warning */
            printf("Found 'X' in unsigned array\n");
        }
    }

    /* VIOLATION: Character constant assignment to wrong types */
    signed char sc = 'M';        /* Character constant to signed char */
    unsigned char uc = 'N';      /* Character constant to unsigned char */
    char c = 'O';                /* OK - appropriate type */

    printf("Character assignments:\n");
    printf("Signed char: %c (%d)\n", sc, sc);
    printf("Unsigned char: %c (%d)\n", uc, uc);
    printf("Plain char: %c (%d)\n", c, c);

    /* VIOLATION: Wide character constants with narrow types */
    char narrow_wide = L'W';     /* Wide character constant to narrow type */
    printf("Wide constant in narrow type: %c\n", narrow_wide);

    /* VIOLATION: Multi-byte character constants */
    int multi_byte = 'AB';       /* Multi-byte character constant */
    printf("Multi-byte constant: %d\n", multi_byte);

    /* VIOLATION: Escape sequences with inappropriate types */
    signed char signed_escapes[] = {'\t', '\n', '\r', '\\'};
    unsigned char unsigned_escapes[] = {'\a', '\b', '\f', '\v'};

    printf("Escape sequences with wrong types:\n");
    for (size_t i = 0; i < 4; i++) {
        printf("Signed escape[%zu]: %d\n", i, signed_escapes[i]);
        printf("Unsigned escape[%zu]: %d\n", i, unsigned_escapes[i]);
    }

    /* VIOLATION: Octal and hex character constants */
    signed char octal_char = '\101';    /* Octal 'A' */
    unsigned char hex_char = '\x42';    /* Hex 'B' */

    printf("Octal/hex constants:\n");
    printf("Octal in signed char: %c (%d)\n", octal_char, octal_char);
    printf("Hex in unsigned char: %c (%d)\n", hex_char, hex_char);

    /* VIOLATION: High-value character constants */
    signed char high_signed = '\xFF';    /* May be negative */
    unsigned char high_unsigned = '\xFF'; /* Always positive */

    printf("High-value constants:\n");
    printf("High in signed char: %d\n", high_signed);
    printf("High in unsigned char: %d\n", high_unsigned);

    /* VIOLATION: Character constants in arithmetic */
    signed char result = 'Z' - 'A';      /* Should use plain char or int */
    printf("Character arithmetic result: %d\n", result);

    /* VIOLATION: Arrays initialized with mixed constant types */
    signed char mixed_array[] = {
        'a',      /* Character constant */
        65,       /* Integer constant */
        '\x41',   /* Hex constant */
        '\101'    /* Octal constant */
    };

    printf("Mixed constant array:\n");
    for (size_t i = 0; i < 4; i++) {
        printf("mixed_array[%zu] = %c (%d)\n", i, mixed_array[i], mixed_array[i]);
    }

    /* VIOLATION: Wide character mixed with narrow */
    wchar_t wide_array[] = {
        L'A',     /* Correct wide character */
        'B',      /* Narrow character in wide context */
        L'\x43',  /* Wide hex character */
        '\x44'    /* Narrow hex in wide context */
    };

    printf("Wide character array with mixed constants:\n");
    for (size_t i = 0; i < 4; i++) {
        printf("wide_array[%zu] = %lc (%d)\n", i, wide_array[i], (int)wide_array[i]);
    }

    /* VIOLATION: Function calls with wrong character constant types */
    signed char function_param = 'P';
    printf("Function call with signed char: ");
    putchar(function_param);  /* May cause warning */
    printf("\n");

    return 0;
}