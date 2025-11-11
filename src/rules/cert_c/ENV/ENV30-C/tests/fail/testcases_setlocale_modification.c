/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: setlocale_modification.c
 *
 * This case demonstrates violations where the return value of setlocale()
 * is modified, leading to undefined behavior.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

/* NON-COMPLIANT: Direct modification of setlocale() return value */
void unsafe_locale_modification(void) {
    char *current_locale = setlocale(LC_ALL, NULL);

    if (current_locale != NULL) {
        /* VIOLATION: Modifying the returned locale string */
        current_locale[0] = 'X';  /* Undefined behavior */
        printf("Modified locale: %s\n", current_locale);
    }
}

/* NON-COMPLIANT: String concatenation on setlocale() result */
void unsafe_locale_concatenation(void) {
    char *locale = setlocale(LC_NUMERIC, "C");

    if (locale != NULL) {
        /* VIOLATION: Appending to locale string */
        strcat(locale, ".modified");  /* Undefined behavior */
        printf("Concatenated locale: %s\n", locale);
    }
}

/* NON-COMPLIANT: Character replacement in locale string */
void unsafe_locale_character_replace(void) {
    char *time_locale = setlocale(LC_TIME, "");

    if (time_locale != NULL) {
        /* VIOLATION: Replacing underscores with dashes */
        for (char *p = time_locale; *p; p++) {
            if (*p == '_') {
                *p = '-';  /* Undefined behavior */
            }
        }
        printf("Modified time locale: %s\n", time_locale);
    }
}

/* NON-COMPLIANT: Overwriting locale string with strcpy */
void unsafe_locale_overwrite(void) {
    char *ctype_locale = setlocale(LC_CTYPE, "C");

    if (ctype_locale != NULL) {
        /* VIOLATION: Overwriting with new string */
        strcpy(ctype_locale, "POSIX");  /* Undefined behavior */
        printf("Overwritten locale: %s\n", ctype_locale);
    }
}

/* NON-COMPLIANT: Truncating locale string */
void unsafe_locale_truncation(void) {
    char *monetary_locale = setlocale(LC_MONETARY, "");

    if (monetary_locale != NULL && strlen(monetary_locale) > 2) {
        /* VIOLATION: Truncating locale string */
        monetary_locale[2] = '\0';  /* Undefined behavior */
        printf("Truncated monetary locale: %s\n", monetary_locale);
    }
}

/* NON-COMPLIANT: Using strtok on locale string */
void unsafe_locale_tokenization(void) {
    char *all_locale = setlocale(LC_ALL, "");

    if (all_locale != NULL) {
        /* VIOLATION: Using strtok which modifies the string */
        char *token = strtok(all_locale, ";");  /* Undefined behavior */
        while (token != NULL) {
            printf("Locale token: %s\n", token);
            token = strtok(NULL, ";");
        }
    }
}

/* NON-COMPLIANT: Converting locale to uppercase */
void unsafe_locale_uppercase(void) {
    char *collate_locale = setlocale(LC_COLLATE, "");

    if (collate_locale != NULL) {
        /* VIOLATION: Converting to uppercase in-place */
        for (char *p = collate_locale; *p; p++) {
            if (*p >= 'a' && *p <= 'z') {
                *p = *p - 'a' + 'A';  /* Undefined behavior */
            }
        }
        printf("Uppercase collate locale: %s\n", collate_locale);
    }
}

/* NON-COMPLIANT: Padding locale string with spaces */
void unsafe_locale_padding(void) {
    char *message_locale = setlocale(LC_MESSAGES, "C");

    if (message_locale != NULL) {
        size_t len = strlen(message_locale);
        /* VIOLATION: Adding padding characters */
        memset(message_locale + len, ' ', 3);  /* Undefined behavior */
        message_locale[len + 3] = '\0';
        printf("Padded message locale: '%s'\n", message_locale);
    }
}

/* NON-COMPLIANT: Multiple setlocale calls with string modification */
void unsafe_multiple_locale_calls(void) {
    char *first_locale = setlocale(LC_ALL, "C");
    char *second_locale = setlocale(LC_NUMERIC, "");

    if (first_locale != NULL) {
        /* VIOLATION: Modifying first locale after second call */
        /* Note: first_locale might already be invalid due to static buffer reuse */
        first_locale[0] = 'M';  /* Undefined behavior */
        printf("Modified first locale: %s\n", first_locale);
    }

    if (second_locale != NULL) {
        /* VIOLATION: Modifying second locale */
        strcat(second_locale, "_MOD");  /* Undefined behavior */
        printf("Modified second locale: %s\n", second_locale);
    }
}

int main(void) {
    printf("=== ENV30-C setlocale() Modification Violations ===\n");

    printf("\n1. Unsafe locale modification:\n");
    unsafe_locale_modification();

    printf("\n2. Unsafe locale concatenation:\n");
    unsafe_locale_concatenation();

    printf("\n3. Unsafe locale character replace:\n");
    unsafe_locale_character_replace();

    printf("\n4. Unsafe locale overwrite:\n");
    unsafe_locale_overwrite();

    printf("\n5. Unsafe locale truncation:\n");
    unsafe_locale_truncation();

    printf("\n6. Unsafe locale tokenization:\n");
    unsafe_locale_tokenization();

    printf("\n7. Unsafe locale uppercase:\n");
    unsafe_locale_uppercase();

    printf("\n8. Unsafe locale padding:\n");
    unsafe_locale_padding();

    printf("\n9. Unsafe multiple locale calls:\n");
    unsafe_multiple_locale_calls();

    return 0;
}