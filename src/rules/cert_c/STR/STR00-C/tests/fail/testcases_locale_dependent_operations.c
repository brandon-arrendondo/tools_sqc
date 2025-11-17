/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: locale_dependent_operations.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * character types with locale-dependent operations, leading to
 * inconsistent behavior across different locale settings.
 */

#include <stdio.h>
#include <locale.h>
#include <ctype.h>
#include <string.h>

int main(void) {
    /* Set a locale that may have extended character sets */
    setlocale(LC_ALL, "");

    printf("Current locale: %s\n", setlocale(LC_ALL, NULL));

    /* VIOLATION: Locale-dependent character operations with signed char */
    signed char locale_text[] = "Café, naïve, résumé, piñata";

    printf("\nLocale operations with signed char:\n");
    printf("Text: %s\n", locale_text);  /* Warning */

    /* VIOLATION: Character-by-character locale analysis */
    for (size_t i = 0; locale_text[i] != '\0'; i++) {
        signed char c = locale_text[i];

        printf("Character %zu: '%c' (value: %d)\n", i,
               (c >= 32 && c < 127) ? c : '?', c);

        /* VIOLATION: Locale-dependent character classification */
        if (isalpha(c)) {  /* Undefined behavior for negative values */
            printf("  Is alphabetic in current locale\n");
        }

        if (isupper(c)) {  /* Undefined behavior for negative values */
            printf("  Is uppercase in current locale\n");
        }

        if (islower(c)) {  /* Undefined behavior for negative values */
            printf("  Is lowercase in current locale\n");
        }
    }

    /* VIOLATION: Case conversion with locale dependency */
    signed char case_test[] = "Mixed Case Text";
    printf("\nCase conversion with signed char:\n");
    printf("Original: %s\n", case_test);  /* Warning */

    /* Convert to uppercase */
    for (size_t i = 0; case_test[i] != '\0'; i++) {
        signed char c = case_test[i];
        if (islower(c)) {  /* Potential undefined behavior */
            case_test[i] = toupper(c);  /* toupper returns int */
        }
    }
    printf("Uppercase: %s\n", case_test);  /* Warning */

    /* VIOLATION: String comparison with locale effects */
    unsigned char string1[] = "apple";
    unsigned char string2[] = "Apple";

    printf("\nLocale-dependent string comparison:\n");

    /* Basic comparison */
    int basic_cmp = strcmp((char*)string1, (char*)string2);  /* Warning */
    printf("Basic strcmp result: %d\n", basic_cmp);

    /* VIOLATION: Manual case-insensitive comparison */
    int case_insensitive_equal = 1;
    size_t len1 = strlen((char*)string1);
    size_t len2 = strlen((char*)string2);

    if (len1 == len2) {
        for (size_t i = 0; i < len1; i++) {
            /* VIOLATION: Character comparison with type issues */
            unsigned char c1 = tolower(string1[i]);  /* tolower returns int */
            unsigned char c2 = tolower(string2[i]);  /* tolower returns int */

            if (c1 != c2) {
                case_insensitive_equal = 0;
                break;
            }
        }
    } else {
        case_insensitive_equal = 0;
    }

    printf("Case-insensitive equal: %s\n", case_insensitive_equal ? "yes" : "no");

    /* VIOLATION: Numeric character handling with locale */
    signed char numeric_text[] = "Price: $123.45 (€456.78)";

    printf("\nNumeric character analysis:\n");
    printf("Text: %s\n", numeric_text);  /* Warning */

    int digit_count = 0;
    int punct_count = 0;

    for (size_t i = 0; numeric_text[i] != '\0'; i++) {
        signed char c = numeric_text[i];

        /* VIOLATION: Character classification with potential undefined behavior */
        if (isdigit(c)) {
            digit_count++;
        } else if (ispunct(c)) {
            punct_count++;
        }
    }

    printf("Digits found: %d\n", digit_count);
    printf("Punctuation found: %d\n", punct_count);

    /* VIOLATION: Character encoding assumptions */
    char encoding_test[] = "ASCII vs Extended: \xC0\xC1\xC2";  /* May be negative */

    printf("\nCharacter encoding assumptions:\n");
    for (size_t i = 0; encoding_test[i] != '\0'; i++) {
        char c = encoding_test[i];

        printf("Character %zu: ", i);
        if (c >= 0 && c < 128) {
            printf("ASCII (%d)\n", c);
        } else if (c < 0) {
            printf("Negative value (%d) - sign extension issue\n", c);
        } else {
            printf("Extended (%d)\n", c);
        }

        /* VIOLATION: Locale-dependent interpretation */
        if (isprint(c)) {  /* Undefined for negative values */
            printf("  Printable in current locale\n");
        }
    }

    /* VIOLATION: Wide character conversion issues */
    char narrow_source[] = "Narrow string with accents: café";
    unsigned char *narrow_ptr = (unsigned char*)narrow_source;

    printf("\nWide character conversion:\n");
    printf("Source: %s\n", narrow_ptr);  /* Warning */

    /* VIOLATION: Manual wide character "conversion" */
    wchar_t wide_dest[100];
    size_t conv_count = 0;

    for (size_t i = 0; narrow_source[i] != '\0' && conv_count < 99; i++) {
        /* VIOLATION: Simple cast loses encoding information */
        wide_dest[conv_count++] = (wchar_t)narrow_source[i];
    }
    wide_dest[conv_count] = L'\0';

    printf("\"Converted\" wide string: %ls\n", wide_dest);

    /* VIOLATION: Collation with character type issues */
    signed char *words[] = {
        "apple", "Apple", "café", "Café", "naïve", "Naïve"
    };
    int word_count = 6;

    printf("\nWord collation issues:\n");
    for (int i = 0; i < word_count; i++) {
        printf("Word %d: %s\n", i, words[i]);  /* Warning */

        /* VIOLATION: Simple sorting comparison */
        for (int j = i + 1; j < word_count; j++) {
            int cmp = strcmp((char*)words[i], (char*)words[j]);  /* Warning */
            if (cmp < 0) {
                printf("  '%s' < '%s'\n", words[i], words[j]);  /* Warning */
            } else if (cmp > 0) {
                printf("  '%s' > '%s'\n", words[i], words[j]);  /* Warning */
            }
        }
    }

    /* VIOLATION: Character frequency analysis with locale issues */
    unsigned char text_sample[] = "Sample text with various characters: àáâãäåæçèéêë";
    int char_freq[256] = {0};

    printf("\nCharacter frequency analysis:\n");
    for (size_t i = 0; text_sample[i] != '\0'; i++) {
        unsigned char c = text_sample[i];
        char_freq[c]++;
    }

    /* Display frequencies for extended characters */
    printf("Extended character frequencies:\n");
    for (int i = 128; i < 256; i++) {
        if (char_freq[i] > 0) {
            printf("Character \\x%02X: %d times\n", i, char_freq[i]);
        }
    }

    /* VIOLATION: Multi-byte character boundary issues */
    char utf8_text[] = "UTF-8: 你好世界";  /* Chinese characters in UTF-8 */
    signed char *utf8_ptr = (signed char*)utf8_text;

    printf("\nUTF-8 boundary issues:\n");
    printf("UTF-8 text: %s\n", utf8_ptr);  /* Warning */

    /* VIOLATION: Byte-by-byte processing of multi-byte characters */
    for (size_t i = 0; utf8_text[i] != '\0'; i++) {
        signed char byte = utf8_text[i];
        printf("Byte %zu: %d (0x%02X)\n", i, byte, (unsigned char)byte);

        /* VIOLATION: Treating UTF-8 bytes as individual characters */
        if (byte > 0) {
            printf("  ASCII byte\n");
        } else {
            printf("  Multi-byte UTF-8 sequence byte\n");
        }
    }

    return 0;
}