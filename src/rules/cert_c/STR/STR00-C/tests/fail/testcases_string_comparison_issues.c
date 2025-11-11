/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: string_comparison_issues.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * character types for string comparison operations, leading to
 * sign-extension issues and incorrect comparison results.
 */

#include <stdio.h>
#include <string.h>

/* VIOLATION: Function using signed char for string comparison */
int compare_signed_strings(const signed char *s1, const signed char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    /* VIOLATION: Sign extension may cause incorrect comparison */
    return *s1 - *s2;  /* Problematic for high-bit characters */
}

int main(void) {
    /* VIOLATION: Using signed char for strings with extended characters */
    signed char string1[] = "Hello\xFF";  /* \xFF may be negative */
    signed char string2[] = "Hello\x01";

    printf("Comparing strings with signed char:\n");
    printf("String 1: ");
    for (size_t i = 0; string1[i] != '\0'; i++) {
        printf("0x%02X ", (unsigned char)string1[i]);
    }
    printf("\n");

    printf("String 2: ");
    for (size_t i = 0; string2[i] != '\0'; i++) {
        printf("0x%02X ", (unsigned char)string2[i]);
    }
    printf("\n");

    /* VIOLATION: Comparison result affected by sign extension */
    int result = compare_signed_strings(string1, string2);
    printf("Comparison result: %d\n", result);

    /* VIOLATION: Character-by-character comparison with sign issues */
    const char *str_a = "Test\x80";  /* High-bit character */
    const char *str_b = "Test\x7F";

    printf("\nCharacter-by-character comparison:\n");
    for (size_t i = 0; str_a[i] != '\0' && str_b[i] != '\0'; i++) {
        char ca = str_a[i];
        char cb = str_b[i];

        printf("Position %zu: '%c'(%d) vs '%c'(%d)\n",
               i, ca, ca, cb, cb);

        /* VIOLATION: Direct comparison may give wrong results */
        if (ca > cb) {
            printf("  First string character is greater\n");
        } else if (ca < cb) {
            printf("  Second string character is greater\n");
        } else {
            printf("  Characters are equal\n");
        }
    }

    /* VIOLATION: memcmp with strings containing high-bit characters */
    unsigned char binary1[] = {0x80, 0x81, 0x82, 0x00};
    signed char binary2[] = {0x80, 0x81, 0x82, 0x00};  /* May be negative */

    printf("\nBinary comparison:\n");
    int memcmp_result = memcmp(binary1, binary2, 4);
    printf("memcmp result: %d\n", memcmp_result);

    /* VIOLATION: String sorting with sign-dependent behavior */
    char *strings[] = {
        "Apple",
        "Banana",
        "\x80Special",  /* High-bit character */
        "Zebra"
    };

    printf("\nString sorting issues:\n");
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            /* VIOLATION: strcmp with sign-dependent characters */
            int cmp = strcmp(strings[i], strings[j]);
            printf("'%s' vs '%s': %d\n", strings[i], strings[j], cmp);
        }
    }

    /* VIOLATION: Case-insensitive comparison with sign issues */
    char mixed_case1[] = "Hello\x90World";  /* High-bit character */
    char mixed_case2[] = "hello\x90world";

    printf("\nCase comparison with high-bit characters:\n");

    /* Manual case-insensitive comparison - problematic */
    int case_diff = 0;
    for (size_t i = 0; mixed_case1[i] != '\0' && mixed_case2[i] != '\0'; i++) {
        char c1 = mixed_case1[i];
        char c2 = mixed_case2[i];

        /* Convert to lowercase - problematic for high-bit chars */
        if (c1 >= 'A' && c1 <= 'Z') c1 += 32;
        if (c2 >= 'A' && c2 <= 'Z') c2 += 32;

        if (c1 != c2) {
            case_diff = c1 - c2;  /* Sign extension issues */
            break;
        }
    }

    printf("Case-insensitive comparison result: %d\n", case_diff);

    return 0;
}