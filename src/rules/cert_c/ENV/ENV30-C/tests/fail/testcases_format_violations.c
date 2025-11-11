/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: format_violations.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

/* NON-COMPLIANT: Format string modification */
void unsafe_format_modification(void) {
    char *locale = setlocale(LC_ALL, "C");
    if (locale) {
        /* VIOLATION: Using as format string and modifying */
        sprintf(locale, "fmt_%s", "test");  /* Undefined behavior */
        printf("Modified format: %s\n", locale);
    }
}

/* NON-COMPLIANT: Locale format modification */
void unsafe_locale_format(void) {
    struct lconv *lc = localeconv();
    if (lc && lc->decimal_point) {
        /* VIOLATION: Changing decimal point format */
        strcpy(lc->decimal_point, ":");  /* Undefined behavior */
        printf("New decimal: %s\n", lc->decimal_point);
    }
}

int main(void) {
    unsafe_format_modification();
    unsafe_locale_format();
    return 0;
}