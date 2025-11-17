/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: unicode_handling_errors.c
 *
 * This case demonstrates a violation of STR00-C by inappropriately
 * handling Unicode and multi-byte character sequences with wrong
 * character types, leading to encoding corruption and data loss.
 */

#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

int main(void) {
    /* Set UTF-8 locale if available */
    setlocale(LC_ALL, "");

    printf("Unicode handling with inappropriate character types:\n\n");

    /* VIOLATION: UTF-8 multi-byte sequences with signed char */
    signed char utf8_string[] = "Hello 世界 🌍 Café"; /* Contains multi-byte UTF-8 */

    printf("UTF-8 string with signed char:\n");
    printf("String: %s\n", utf8_string);  /* Warning */
    printf("Byte analysis:\n");

    /* VIOLATION: Byte-by-byte analysis losing Unicode structure */
    for (size_t i = 0; utf8_string[i] != '\0'; i++) {
        signed char byte = utf8_string[i];
        printf("Byte %zu: %d (0x%02X) ", i, byte, (unsigned char)byte);

        if (byte < 0) {
            printf("(negative - UTF-8 continuation byte)\n");
        } else if (byte >= 32 && byte < 127) {
            printf("('%c' - ASCII)\n", byte);
        } else {
            printf("(control/extended)\n");
        }
    }

    /* VIOLATION: Character counting with multi-byte sequences */
    size_t byte_length = strlen((char*)utf8_string);
    printf("Byte length: %zu (not character count)\n", byte_length);

    /* VIOLATION: Truncation that breaks UTF-8 sequences */
    signed char truncated[20];
    strncpy(truncated, utf8_string, 19);  /* Warning */
    truncated[19] = '\0';

    printf("Truncated string: %s\n", truncated);  /* Warning - may be corrupted */

    /* VIOLATION: Wide character to narrow conversion */
    printf("\nWide to narrow conversion issues:\n");

    wchar_t wide_text[] = L"Unicode: αβγδε ñáéíóú 中文 日本語";
    printf("Wide string: %ls\n", wide_text);

    /* VIOLATION: Converting wide characters to signed char */
    signed char narrow_buffer[200];
    size_t converted = 0;

    for (size_t i = 0; wide_text[i] != L'\0' && converted < 199; i++) {
        wchar_t wc = wide_text[i];

        /* VIOLATION: Truncating wide characters */
        if (wc <= 127) {
            narrow_buffer[converted++] = (signed char)wc;  /* Data loss for non-ASCII */
        } else {
            narrow_buffer[converted++] = '?';  /* Replace with placeholder */
        }
    }
    narrow_buffer[converted] = '\0';

    printf("Converted to signed char: %s\n", narrow_buffer);  /* Warning */

    /* VIOLATION: Unicode escape sequence handling */
    printf("\nUnicode escape sequence mishandling:\n");

    unsigned char escaped_unicode[] = "Text with \\u0041\\u0042\\u0043 and \\u4E2D\\u6587";
    printf("String with escapes: %s\n", escaped_unicode);  /* Warning */

    /* VIOLATION: Manual escape processing with wrong types */
    for (size_t i = 0; escaped_unicode[i] != '\0'; i++) {
        if (escaped_unicode[i] == '\\' && escaped_unicode[i+1] == 'u') {
            printf("Found Unicode escape at position %zu\n", i);

            /* VIOLATION: Hex parsing with character type issues */
            if (i + 5 < strlen((char*)escaped_unicode)) {
                unsigned char hex_chars[5];
                strncpy((char*)hex_chars, (char*)&escaped_unicode[i+2], 4);  /* Warning */
                hex_chars[4] = '\0';
                printf("  Hex code: %s\n", hex_chars);  /* Warning */

                /* Simple hex to decimal conversion (incomplete) */
                int unicode_value = 0;
                for (int j = 0; j < 4; j++) {
                    unsigned char c = hex_chars[j];
                    if (c >= '0' && c <= '9') {
                        unicode_value = unicode_value * 16 + (c - '0');
                    } else if (c >= 'A' && c <= 'F') {
                        unicode_value = unicode_value * 16 + (c - 'A' + 10);
                    }
                }
                printf("  Unicode value: U+%04X\n", unicode_value);
            }
        }
    }

    /* VIOLATION: BOM (Byte Order Mark) handling */
    printf("\nBOM handling issues:\n");

    /* UTF-8 BOM: EF BB BF */
    signed char utf8_with_bom[] = "\xEF\xBB\xBF" "UTF-8 text with BOM";
    printf("UTF-8 with BOM: %s\n", utf8_with_bom);  /* Warning */

    /* VIOLATION: BOM detection with signed char */
    if (utf8_with_bom[0] == (signed char)0xEF &&
        utf8_with_bom[1] == (signed char)0xBB &&
        utf8_with_bom[2] == (signed char)0xBF) {
        printf("UTF-8 BOM detected (may fail on signed char systems)\n");
    }

    /* Skip BOM for processing */
    signed char *content_start = utf8_with_bom + 3;
    printf("Content after BOM: %s\n", content_start);  /* Warning */

    /* VIOLATION: Emoji and surrogate pairs */
    printf("\nEmoji handling issues:\n");

    char emoji_text[] = "Emojis: 😀 🌟 🚀 ❤️ 🎉";
    printf("Emoji text: %s\n", emoji_text);

    /* VIOLATION: Byte counting vs character counting */
    printf("Byte count: %zu\n", strlen(emoji_text));

    /* Count emoji-like sequences (very simplified) */
    int potential_emoji = 0;
    for (size_t i = 0; emoji_text[i] != '\0'; i++) {
        /* VIOLATION: Checking for high-bit bytes */
        if ((unsigned char)emoji_text[i] >= 0xF0) {  /* UTF-8 4-byte sequence start */
            potential_emoji++;
        }
    }
    printf("Potential emoji sequences: %d\n", potential_emoji);

    /* VIOLATION: Case conversion with Unicode */
    printf("\nCase conversion issues:\n");

    signed char international_text[] = "Straße CAFÉ türkçe";
    printf("Original: %s\n", international_text);  /* Warning */

    /* VIOLATION: Simple ASCII-only case conversion */
    for (size_t i = 0; international_text[i] != '\0'; i++) {
        signed char c = international_text[i];

        /* This only works for ASCII and breaks Unicode */
        if (c >= 'a' && c <= 'z') {
            international_text[i] = c - 32;  /* Convert to uppercase */
        } else if (c >= 'A' && c <= 'Z') {
            international_text[i] = c + 32;  /* Convert to lowercase */
        }
    }

    printf("After ASCII case conversion: %s\n", international_text);  /* Warning */

    /* VIOLATION: String comparison with Unicode normalization issues */
    printf("\nUnicode normalization issues:\n");

    /* These may look the same but have different byte representations */
    char composed[] = "café";      /* é as single character */
    char decomposed[] = "cafe\xCC\x81";  /* e + combining acute accent */

    printf("Composed: %s (length: %zu)\n", composed, strlen(composed));
    printf("Decomposed: %s (length: %zu)\n", decomposed, strlen(decomposed));

    /* VIOLATION: Direct byte comparison */
    if (strcmp(composed, decomposed) == 0) {
        printf("Strings are identical\n");
    } else {
        printf("Strings differ (normalization issue)\n");
    }

    /* VIOLATION: Manual Unicode validation with wrong types */
    printf("\nManual Unicode validation:\n");

    unsigned char test_sequences[] = {
        0xC2, 0xA9,           /* Valid 2-byte: © */
        0xE2, 0x82, 0xAC,     /* Valid 3-byte: € */
        0xF0, 0x9F, 0x98, 0x80, /* Valid 4-byte: 😀 */
        0xC0, 0x80,           /* Invalid overlong */
        0x00
    };

    printf("Validating UTF-8 sequences:\n");
    for (size_t i = 0; test_sequences[i] != 0; ) {
        unsigned char first_byte = test_sequences[i];
        printf("Sequence starting with 0x%02X: ", first_byte);

        /* VIOLATION: Simplified UTF-8 validation */
        if (first_byte < 0x80) {
            printf("ASCII\n");
            i++;
        } else if ((first_byte & 0xE0) == 0xC0) {
            printf("2-byte sequence\n");
            i += 2;
        } else if ((first_byte & 0xF0) == 0xE0) {
            printf("3-byte sequence\n");
            i += 3;
        } else if ((first_byte & 0xF8) == 0xF0) {
            printf("4-byte sequence\n");
            i += 4;
        } else {
            printf("Invalid start byte\n");
            i++;
        }
    }

    return 0;
}