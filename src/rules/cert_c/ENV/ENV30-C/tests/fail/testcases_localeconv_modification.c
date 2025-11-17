/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: localeconv_modification.c
 *
 * This case demonstrates violations where the return value of localeconv()
 * is modified, leading to undefined behavior.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

/* NON-COMPLIANT: Direct modification of localeconv() structure members */
void unsafe_localeconv_modification(void) {
    struct lconv *locale_info = localeconv();

    if (locale_info != NULL) {
        /* VIOLATION: Modifying decimal_point string */
        if (locale_info->decimal_point != NULL) {
            locale_info->decimal_point[0] = '#';  /* Undefined behavior */
            printf("Modified decimal point: %s\n", locale_info->decimal_point);
        }
    }
}

/* NON-COMPLIANT: Overwriting localeconv() string members */
void unsafe_localeconv_string_overwrite(void) {
    struct lconv *locale_info = localeconv();

    if (locale_info != NULL) {
        /* VIOLATION: Overwriting thousands_sep */
        if (locale_info->thousands_sep != NULL) {
            strcpy(locale_info->thousands_sep, "_");  /* Undefined behavior */
            printf("Modified thousands separator: %s\n", locale_info->thousands_sep);
        }
    }
}

/* NON-COMPLIANT: Modifying currency symbol */
void unsafe_currency_symbol_modification(void) {
    setlocale(LC_MONETARY, "");  /* Set to system locale */
    struct lconv *locale_info = localeconv();

    if (locale_info != NULL && locale_info->currency_symbol != NULL) {
        /* VIOLATION: Modifying currency symbol */
        strcat(locale_info->currency_symbol, "*");  /* Undefined behavior */
        printf("Modified currency symbol: %s\n", locale_info->currency_symbol);
    }
}

/* NON-COMPLIANT: Modifying international currency symbol */
void unsafe_int_curr_symbol_modification(void) {
    struct lconv *locale_info = localeconv();

    if (locale_info != NULL && locale_info->int_curr_symbol != NULL) {
        /* VIOLATION: Changing international currency symbol */
        strcpy(locale_info->int_curr_symbol, "XXX ");  /* Undefined behavior */
        printf("Modified int currency: %s\n", locale_info->int_curr_symbol);
    }
}

/* NON-COMPLIANT: Modifying monetary formatting strings */
void unsafe_monetary_format_modification(void) {
    struct lconv *locale_info = localeconv();

    if (locale_info != NULL) {
        /* VIOLATION: Modifying positive sign */
        if (locale_info->positive_sign != NULL) {
            strcpy(locale_info->positive_sign, "PLUS");  /* Undefined behavior */
        }

        /* VIOLATION: Modifying negative sign */
        if (locale_info->negative_sign != NULL) {
            strcpy(locale_info->negative_sign, "MINUS");  /* Undefined behavior */
        }

        printf("Modified signs: pos='%s', neg='%s'\n",
               locale_info->positive_sign ?: "(null)",
               locale_info->negative_sign ?: "(null)");
    }
}

int main(void) {
    printf("=== ENV30-C localeconv() Modification Violations ===\n");

    printf("\n1. Unsafe localeconv modification:\n");
    unsafe_localeconv_modification();

    printf("\n2. Unsafe localeconv string overwrite:\n");
    unsafe_localeconv_string_overwrite();

    printf("\n3. Unsafe currency symbol modification:\n");
    unsafe_currency_symbol_modification();

    printf("\n4. Unsafe int currency symbol modification:\n");
    unsafe_int_curr_symbol_modification();

    printf("\n5. Unsafe monetary format modification:\n");
    unsafe_monetary_format_modification();

    return 0;
}