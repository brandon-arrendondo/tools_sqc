/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: printf_format_mismatch.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * character types with printf format specifiers, leading to format
 * string mismatches and potential undefined behavior.
 */

#include <stdio.h>

int main(void) {
    /* VIOLATION: Using signed/unsigned char with %s format */
    signed char signed_string[] = "Signed char string";
    unsigned char unsigned_string[] = "Unsigned char string";

    printf("Format specifier mismatches:\n");

    /* VIOLATION: %s expects char*, not signed char* or unsigned char* */
    printf("Signed string: %s\n", signed_string);      /* Warning */
    printf("Unsigned string: %s\n", unsigned_string);  /* Warning */

    /* VIOLATION: Character format specifiers with wrong types */
    signed char sc = -50;
    unsigned char uc = 200;
    char c = 'A';

    /* These may produce unexpected output */
    printf("Signed char with %%c: %c\n", sc);    /* May display wrong character */
    printf("Unsigned char with %%c: %c\n", uc);  /* May display wrong character */

    /* VIOLATION: Integer format with character types */
    printf("Character as decimal:\n");
    printf("  Plain char 'A': %d\n", c);        /* OK */
    printf("  Signed char -50: %d\n", sc);       /* May show unexpected value */
    printf("  Unsigned char 200: %d\n", uc);     /* May show unexpected value */

    /* VIOLATION: Using %u with potentially signed values */
    printf("Using %%u format:\n");
    printf("  Signed char: %u\n", sc);           /* Wrong format for signed */
    printf("  Plain char: %u\n", c);             /* Sign-dependent */

    /* VIOLATION: Hexadecimal format issues */
    printf("Hexadecimal representation:\n");
    printf("  Signed char: 0x%x\n", sc);         /* Sign extension issues */
    printf("  Unsigned char: 0x%x\n", uc);       /* May be OK */
    printf("  Plain char: 0x%x\n", c);           /* Sign-dependent */

    /* VIOLATION: String operations with mixed character types */
    signed char *sp = signed_string;
    unsigned char *up = unsigned_string;

    printf("\nPointer format issues:\n");
    printf("String at %p: %s\n", (void*)sp, sp);        /* Type warning */
    printf("String at %p: %s\n", (void*)up, up);        /* Type warning */

    /* VIOLATION: Wide character format mismatches */
    wchar_t wide_char = L'ñ';
    wchar_t wide_string[] = L"Wide string";

    /* Wrong format specifiers for wide characters */
    printf("Wide char with %%c: %c\n", wide_char);      /* Data loss */
    printf("Wide string with %%s: %s\n", wide_string);  /* Wrong format */

    /* VIOLATION: Array printing with wrong assumptions */
    char byte_array[] = {65, 66, 67, -1, 0};  /* Mixed positive/negative */

    printf("\nByte array printing:\n");
    for (size_t i = 0; i < sizeof(byte_array); i++) {
        /* Sign-dependent output */
        printf("byte_array[%zu] = %d (char: %c)\n",
               i, byte_array[i], byte_array[i]);
    }

    /* VIOLATION: Length modifier mismatches */
    char short_char = 'X';
    printf("Character with wrong length modifier: %ld\n", (long)short_char);

    /* VIOLATION: Precision specifiers with characters */
    printf("Precision with character: %.2c\n", c);      /* Invalid precision */
    printf("Width with character: %5c\n", c);           /* OK but unusual */

    /* VIOLATION: Octal format with signed characters */
    signed char octal_chars[] = {-1, -8, 64, 127};
    printf("\nOctal representation issues:\n");
    for (size_t i = 0; i < 4; i++) {
        printf("Character %zu: %o\n", i, octal_chars[i]);  /* Sign extension */
    }

    return 0;
}