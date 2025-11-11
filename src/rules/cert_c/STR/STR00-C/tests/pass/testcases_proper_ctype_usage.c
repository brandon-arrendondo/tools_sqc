/*
 * Rule: STR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR00-C violation
 */

/*
 * CERT C STR00-C Pass Case: proper_ctype_usage.c
 *
 * This case demonstrates compliant code that properly uses ctype.h
 * functions with appropriate character types and casting, ensuring
 * defined behavior for all character values.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

int main(void) {
    printf("Proper ctype function usage:\n\n");

    /* COMPLIANT: Using ctype functions with proper casting */
    const char *text = "Hello World 123!@#";

    printf("Text analysis: %s\n", text);
    printf("Character-by-character analysis:\n");

    for (size_t i = 0; text[i] != '\0'; i++) {
        /* COMPLIANT: Cast to unsigned char before passing to ctype functions */
        int ch = (unsigned char)text[i];
        char c = text[i];

        printf("'%c': ", c);

        /* COMPLIANT: All ctype function calls use properly cast values */
        if (isalpha(ch)) {
            printf("alpha ");
        }
        if (isdigit(ch)) {
            printf("digit ");
        }
        if (isalnum(ch)) {
            printf("alnum ");
        }
        if (ispunct(ch)) {
            printf("punct ");
        }
        if (isspace(ch)) {
            printf("space ");
        }
        if (isprint(ch)) {
            printf("print ");
        }
        if (iscntrl(ch)) {
            printf("ctrl ");
        }

        printf("\n");
    }

    /* COMPLIANT: Character classification with extended ASCII */
    printf("\nExtended ASCII character testing:\n");

    /* Test various character values including extended ASCII */
    unsigned char test_chars[] = {
        65,   /* 'A' */
        97,   /* 'a' */
        48,   /* '0' */
        32,   /* space */
        9,    /* tab */
        128,  /* extended ASCII */
        200,  /* extended ASCII */
        255   /* extended ASCII */
    };

    for (size_t i = 0; i < sizeof(test_chars); i++) {
        /* COMPLIANT: Using unsigned char directly as int parameter */
        int ch = test_chars[i];

        printf("Character value %u: ", test_chars[i]);

        if (isprint(ch)) {
            printf("'%c' - ", ch);
        } else {
            printf("(non-printable) - ");
        }

        printf("alpha:%d digit:%d punct:%d space:%d\n",
               isalpha(ch), isdigit(ch), ispunct(ch), isspace(ch));
    }

    /* COMPLIANT: Case conversion with proper types */
    printf("\nCase conversion:\n");

    char mixed_text[] = "MiXeD cAsE tExT 123!";
    printf("Original: %s\n", mixed_text);

    /* Convert to all uppercase */
    for (size_t i = 0; mixed_text[i] != '\0'; i++) {
        int ch = (unsigned char)mixed_text[i];
        if (islower(ch)) {
            /* COMPLIANT: toupper returns int, safe to cast after validation */
            mixed_text[i] = (char)toupper(ch);
        }
    }

    printf("Uppercase: %s\n", mixed_text);

    /* Convert to all lowercase */
    for (size_t i = 0; mixed_text[i] != '\0'; i++) {
        int ch = (unsigned char)mixed_text[i];
        if (isupper(ch)) {
            /* COMPLIANT: tolower returns int, safe to cast after validation */
            mixed_text[i] = (char)tolower(ch);
        }
    }

    printf("Lowercase: %s\n", mixed_text);

    /* COMPLIANT: Character validation function */
    printf("\nCharacter validation:\n");

    const char *test_strings[] = {
        "ValidIdentifier",
        "valid_123",
        "123Invalid",
        "invalid-name",
        "a",
        "",
        "has spaces"
    };

    for (size_t i = 0; i < 7; i++) {
        const char *str = test_strings[i];
        int valid = 1;

        printf("Testing '%s': ", str);

        if (strlen(str) == 0) {
            valid = 0;
            printf("empty string");
        } else {
            /* First character must be letter or underscore */
            int first_ch = (unsigned char)str[0];
            if (!isalpha(first_ch) && first_ch != '_') {
                valid = 0;
                printf("invalid first character");
            } else {
                /* Remaining characters must be alphanumeric or underscore */
                for (size_t j = 1; str[j] != '\0'; j++) {
                    int ch = (unsigned char)str[j];
                    if (!isalnum(ch) && ch != '_') {
                        valid = 0;
                        printf("invalid character at position %zu", j);
                        break;
                    }
                }
            }
        }

        if (valid) {
            printf("valid identifier");
        }
        printf("\n");
    }

    /* COMPLIANT: Whitespace trimming */
    printf("\nWhitespace trimming:\n");

    char padded_text[] = "   \t  Hello World  \t  \n";
    printf("Original: '%s'\n", padded_text);

    /* Find start of non-whitespace */
    char *start = padded_text;
    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }

    /* Find end of non-whitespace */
    char *end = padded_text + strlen(padded_text) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }

    /* Null-terminate at end */
    *(end + 1) = '\0';

    printf("Trimmed: '%s'\n", start);

    /* COMPLIANT: Character counting by category */
    printf("\nCharacter category counting:\n");

    const char *sample_text = "Sample Text 123 with Punctuation!@# and\tWhitespace\n";
    printf("Sample: %s", sample_text);

    int counts[8] = {0};  /* alpha, digit, punct, space, upper, lower, print, ctrl */

    for (size_t i = 0; sample_text[i] != '\0'; i++) {
        int ch = (unsigned char)sample_text[i];

        if (isalpha(ch)) counts[0]++;
        if (isdigit(ch)) counts[1]++;
        if (ispunct(ch)) counts[2]++;
        if (isspace(ch)) counts[3]++;
        if (isupper(ch)) counts[4]++;
        if (islower(ch)) counts[5]++;
        if (isprint(ch)) counts[6]++;
        if (iscntrl(ch)) counts[7]++;
    }

    printf("Alphabetic: %d\n", counts[0]);
    printf("Digits: %d\n", counts[1]);
    printf("Punctuation: %d\n", counts[2]);
    printf("Whitespace: %d\n", counts[3]);
    printf("Uppercase: %d\n", counts[4]);
    printf("Lowercase: %d\n", counts[5]);
    printf("Printable: %d\n", counts[6]);
    printf("Control: %d\n", counts[7]);

    /* COMPLIANT: Hexadecimal digit validation */
    printf("\nHexadecimal validation:\n");

    const char *hex_strings[] = {
        "1234ABCD",
        "deadbeef",
        "CAFEBABE",
        "123G456",   /* Invalid */
        "xyz789"     /* Invalid */
    };

    for (size_t i = 0; i < 5; i++) {
        const char *hex_str = hex_strings[i];
        int valid_hex = 1;

        printf("'%s': ", hex_str);

        for (size_t j = 0; hex_str[j] != '\0'; j++) {
            int ch = (unsigned char)hex_str[j];
            if (!isxdigit(ch)) {
                valid_hex = 0;
                break;
            }
        }

        printf("%s\n", valid_hex ? "valid hex" : "invalid hex");
    }

    /* COMPLIANT: Password strength checking */
    printf("\nPassword strength validation:\n");

    const char *passwords[] = {
        "weak",
        "StrongPassword123",
        "NoDigits!",
        "nouppercase123!",
        "NOLOWERCASE123!",
        "NoSpecialChars123",
        "Perfect123!"
    };

    for (size_t i = 0; i < 7; i++) {
        const char *pwd = passwords[i];
        int has_upper = 0, has_lower = 0, has_digit = 0, has_special = 0;
        int length = (int)strlen(pwd);

        printf("Password '%s': ", pwd);

        for (size_t j = 0; pwd[j] != '\0'; j++) {
            int ch = (unsigned char)pwd[j];

            if (isupper(ch)) has_upper = 1;
            else if (islower(ch)) has_lower = 1;
            else if (isdigit(ch)) has_digit = 1;
            else if (ispunct(ch)) has_special = 1;
        }

        int score = has_upper + has_lower + has_digit + has_special;
        if (length >= 8) score++;

        printf("length:%d score:%d/5 ", length, score);

        if (score >= 4) printf("(strong)");
        else if (score >= 2) printf("(medium)");
        else printf("(weak)");

        printf("\n");
    }

    return 0;
}