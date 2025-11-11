/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: encoding_violations.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

/* NON-COMPLIANT: Encoding modification */
void unsafe_encoding_modification(void) {
    char *lang = getenv("LANG");
    if (lang) {
        /* VIOLATION: Removing encoding suffix */
        char *dot = strrchr(lang, '.');
        if (dot) {
            *dot = '\0';  /* Undefined behavior */
        }
        printf("Language without encoding: %s\n", lang);
    }
}

/* NON-COMPLIANT: Locale encoding change */
void unsafe_locale_encoding_change(void) {
    char *lc_ctype = setlocale(LC_CTYPE, "");
    if (lc_ctype) {
        /* VIOLATION: Replacing encoding */
        char *dot = strrchr(lc_ctype, '.');
        if (dot) {
            strcpy(dot, ".ISO-8859-1");  /* Undefined behavior */
        }
        printf("Changed encoding: %s\n", lc_ctype);
    }
}

int main(void) {
    setenv("LANG", "en_US.UTF-8", 1);

    unsafe_encoding_modification();
    unsafe_locale_encoding_change();
    return 0;
}