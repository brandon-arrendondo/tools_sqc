/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: character_encoding_issues.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * character types when dealing with different character encodings,
 * leading to data corruption and encoding misinterpretation.
 */

#include <stdio.h>
#include <string.h>
#include <locale.h>

int main(void) {
    setlocale(LC_ALL, "");

    /* VIOLATION: Using signed char for UTF-8 encoded data */
    signed char utf8_string[] = "UTF-8: Héllö Wörld! 🌍";  /* Multi-byte characters */

    printf("UTF-8 handling with signed char:\n");
    printf("String: %s\n", utf8_string);  /* Warning */

    /* VIOLATION: Character analysis of UTF-8 with wrong type */
    for (size_t i = 0; utf8_string[i] != '\0'; i++) {
        signed char c = utf8_string[i];
        printf("Byte %zu: %d (0x%02X)\n", i, c, (unsigned char)c);

        /* VIOLATION: Treating UTF-8 bytes as individual characters */
        if (c > 127) {  /* Multi-byte UTF-8 sequences */
            printf("  High-bit byte (part of multi-byte sequence)\n");
        }
    }

    /* VIOLATION: Latin-1 characters with wrong signedness */
    unsigned char latin1_chars[] = {
        0xC0, 0xC1, 0xC2, 0xC3,  /* À Á Â Ã */
        0xE0, 0xE1, 0xE2, 0xE3,  /* à á â ã */
        0x00
    };

    printf("\nLatin-1 character handling:\n");
    for (size_t i = 0; latin1_chars[i] != 0; i++) {
        /* VIOLATION: Assigning to signed char loses high-bit information */
        signed char signed_latin = latin1_chars[i];
        printf("Latin-1 char: unsigned=%d, signed=%d\n",
               latin1_chars[i], signed_latin);
    }

    /* VIOLATION: ASCII assumption with extended characters */
    char mixed_encoding[] = "ASCII + Extended: \x80\x90\xA0\xFF";

    printf("\nMixed encoding analysis:\n");
    for (size_t i = 0; mixed_encoding[i] != '\0'; i++) {
        char c = mixed_encoding[i];

        /* VIOLATION: ASCII-only logic applied to extended characters */
        if (c >= 'A' && c <= 'Z') {
            printf("Character %zu: uppercase ASCII\n", i);
        } else if (c >= 'a' && c <= 'z') {
            printf("Character %zu: lowercase ASCII\n", i);
        } else if (c >= 0x80) {  /* This comparison is sign-dependent */
            printf("Character %zu: extended character\n", i);
        } else {
            printf("Character %zu: other (value=%d)\n", i, c);
        }
    }

    /* VIOLATION: Wide character conversion with data loss */
    wchar_t wide_source[] = L"Wide: αβγδε ñáéíóú 中文";
    char narrow_dest[100];

    printf("\nWide to narrow conversion (data loss):\n");

    /* VIOLATION: Converting wide characters to narrow */
    size_t converted = 0;
    for (size_t i = 0; wide_source[i] != L'\0' && converted < 99; i++) {
        /* VIOLATION: Truncating wide characters to char */
        narrow_dest[converted] = (char)wide_source[i];  /* Data loss */
        converted++;
    }
    narrow_dest[converted] = '\0';

    printf("Original wide string length: %zu\n", wcslen(wide_source));
    printf("Converted narrow string: %s\n", narrow_dest);
    printf("Converted length: %zu\n", strlen(narrow_dest));

    /* VIOLATION: Mixing narrow and wide character operations */
    char narrow_part[] = "Narrow";
    wchar_t wide_part[] = L"Wide";

    /* VIOLATION: Comparing narrow and wide characters */
    if (narrow_part[0] == (char)wide_part[0]) {  /* Wrong comparison */
        printf("First characters match (incorrectly)\n");
    }

    /* VIOLATION: Encoding-specific character manipulation */
    signed char cp1252_chars[] = {
        0x80, 0x82, 0x83, 0x84,  /* CP1252 specific characters */
        0x85, 0x86, 0x87, 0x88,
        0x00
    };

    printf("\nCP1252 character handling with signed char:\n");
    for (size_t i = 0; cp1252_chars[i] != 0; i++) {
        signed char c = cp1252_chars[i];

        /* VIOLATION: Sign extension affects interpretation */
        printf("CP1252 char %zu: as signed=%d, as unsigned=%d\n",
               i, c, (unsigned char)c);

        /* VIOLATION: Character set assumptions */
        if (c > 0) {  /* Sign-dependent logic */
            printf("  Treated as positive character\n");
        } else {
            printf("  Treated as negative (wrong interpretation)\n");
        }
    }

    /* VIOLATION: Buffer operations with encoding assumptions */
    char source_buffer[] = "Source: café résumé naïve";
    signed char dest_buffer[100];

    /* VIOLATION: Copying with type mismatch */
    for (size_t i = 0; i < strlen(source_buffer); i++) {
        dest_buffer[i] = source_buffer[i];  /* Warning */
    }
    dest_buffer[strlen(source_buffer)] = '\0';

    printf("\nBuffer copy with type mismatch:\n");
    printf("Source: %s\n", source_buffer);
    printf("Destination: %s\n", dest_buffer);  /* Warning */

    /* VIOLATION: Locale-dependent character handling */
    unsigned char test_chars[] = {0xE4, 0xF6, 0xFC, 0xDF, 0x00};  /* äöüß in some encodings */

    printf("\nLocale-dependent character analysis:\n");
    for (size_t i = 0; test_chars[i] != 0; i++) {
        /* VIOLATION: Assuming character interpretation */
        char c = (char)test_chars[i];  /* Sign conversion */
        printf("Character %zu: value=%d, interpretation depends on locale\n", i, c);
    }

    return 0;
}