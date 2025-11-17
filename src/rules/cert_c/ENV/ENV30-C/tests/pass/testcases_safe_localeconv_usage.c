/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Pass Case: safe_localeconv_usage.c
 *
 * This case demonstrates compliant usage of localeconv() by properly
 * handling the returned structure without modification.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

/* COMPLIANT: Safe immediate use of localeconv */
void safe_display_locale_info(void) {
    struct lconv *locale_info = localeconv();

    if (locale_info != NULL) {
        /* Safe immediate use without modification */
        printf("Locale information:\n");
        printf("  Decimal point: '%s'\n", locale_info->decimal_point ?: "");
        printf("  Thousands separator: '%s'\n", locale_info->thousands_sep ?: "");
        printf("  Currency symbol: '%s'\n", locale_info->currency_symbol ?: "");
        printf("  International currency: '%s'\n", locale_info->int_curr_symbol ?: "");
    }
}

/* COMPLIANT: Safe copying of locale information */
void safe_copy_locale_info(void) {
    struct lconv *locale_info = localeconv();

    if (locale_info != NULL) {
        /* Create copies of strings for safe modification */
        char *decimal_copy = NULL;
        char *thousands_copy = NULL;
        char *currency_copy = NULL;

        if (locale_info->decimal_point != NULL) {
            decimal_copy = strdup(locale_info->decimal_point);
        }

        if (locale_info->thousands_sep != NULL) {
            thousands_copy = strdup(locale_info->thousands_sep);
        }

        if (locale_info->currency_symbol != NULL) {
            currency_copy = strdup(locale_info->currency_symbol);
        }

        /* Safe to modify the copies */
        printf("Copied and processed locale info:\n");
        if (decimal_copy != NULL) {
            printf("  Decimal point copy: '%s'\n", decimal_copy);
            free(decimal_copy);
        }

        if (thousands_copy != NULL) {
            printf("  Thousands separator copy: '%s'\n", thousands_copy);
            free(thousands_copy);
        }

        if (currency_copy != NULL) {
            printf("  Currency symbol copy: '%s'\n", currency_copy);
            free(currency_copy);
        }
    }
}

/* COMPLIANT: Safe monetary formatting using localeconv */
void safe_monetary_formatting(double amount) {
    struct lconv *lc = localeconv();

    if (lc != NULL) {
        /* Use locale information for safe formatting */
        char formatted_amount[100];

        const char *decimal_point = lc->decimal_point ?: ".";
        const char *thousands_sep = lc->thousands_sep ?: "";
        const char *currency_symbol = lc->currency_symbol ?: "$";

        /* Create formatted string in new buffer */
        if (strlen(thousands_sep) > 0) {
            /* Simplified formatting with thousands separator */
            snprintf(formatted_amount, sizeof(formatted_amount),
                    "%s%.2f", currency_symbol, amount);
        } else {
            snprintf(formatted_amount, sizeof(formatted_amount),
                    "%s%.2f", currency_symbol, amount);
        }

        printf("Formatted amount: %s\n", formatted_amount);
    }
}

/* COMPLIANT: Safe numeric formatting using localeconv */
void safe_numeric_formatting(double number) {
    struct lconv *lc = localeconv();

    if (lc != NULL) {
        char formatted_number[100];
        const char *decimal_point = lc->decimal_point ?: ".";

        /* Build formatted number safely */
        if (strcmp(decimal_point, ".") != 0) {
            /* Need to replace decimal point */
            snprintf(formatted_number, sizeof(formatted_number), "%.3f", number);

            /* Replace '.' with locale decimal point in our buffer */
            char *dot = strchr(formatted_number, '.');
            if (dot != NULL && strlen(decimal_point) == 1) {
                *dot = decimal_point[0];
            }
        } else {
            snprintf(formatted_number, sizeof(formatted_number), "%.3f", number);
        }

        printf("Locale-formatted number: %s\n", formatted_number);
    }
}

/* COMPLIANT: Safe locale information validation */
int safe_validate_locale_info(void) {
    struct lconv *lc = localeconv();

    if (lc == NULL) {
        return 0;
    }

    /* Validate locale information without modifying */
    int valid = 1;

    if (lc->decimal_point == NULL || strlen(lc->decimal_point) == 0) {
        printf("Warning: Invalid decimal point\n");
        valid = 0;
    }

    if (lc->int_curr_symbol != NULL && strlen(lc->int_curr_symbol) != 4) {
        printf("Warning: International currency symbol should be 4 characters\n");
        valid = 0;
    }

    printf("Locale information is %s\n", valid ? "valid" : "questionable");
    return valid;
}

int main(void) {
    printf("=== ENV30-C Safe localeconv() Usage Demo ===\n");

    /* Test with different locales */
    printf("\n=== Testing with C locale ===\n");
    setlocale(LC_ALL, "C");

    printf("\n1. Safe display locale info:\n");
    safe_display_locale_info();

    printf("\n2. Safe copy locale info:\n");
    safe_copy_locale_info();

    printf("\n3. Safe monetary formatting:\n");
    safe_monetary_formatting(1234.56);

    printf("\n4. Safe numeric formatting:\n");
    safe_numeric_formatting(9876.543);

    printf("\n5. Safe locale validation:\n");
    safe_validate_locale_info();

    /* Test with system locale if available */
    printf("\n=== Testing with system locale ===\n");
    if (setlocale(LC_ALL, "") != NULL) {
        printf("Successfully set to system locale\n");

        printf("\n1. Safe display locale info (system):\n");
        safe_display_locale_info();

        printf("\n2. Safe monetary formatting (system):\n");
        safe_monetary_formatting(1234.56);

        printf("\n3. Safe numeric formatting (system):\n");
        safe_numeric_formatting(9876.543);
    } else {
        printf("System locale not available, staying with C locale\n");
    }

    return 0;
}