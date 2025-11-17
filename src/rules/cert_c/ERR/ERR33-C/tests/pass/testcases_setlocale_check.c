/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: PASS
 * Reason: setlocale() return value is properly checked for failure
 */

#include <stdio.h>
#include <locale.h>
#include <string.h>

int main() {
    // Save current locale
    char *current_locale = setlocale(LC_ALL, NULL);
    if (current_locale == NULL) {
        fprintf(stderr, "Failed to get current locale\n");
        return 1;
    }

    // Make a copy since setlocale may modify the returned string
    char saved_locale[256];
    strncpy(saved_locale, current_locale, sizeof(saved_locale) - 1);
    saved_locale[sizeof(saved_locale) - 1] = '\0';

    // Try to set a new locale
    if (setlocale(LC_ALL, "C") == NULL) {
        fprintf(stderr, "Failed to set locale to C\n");
        return 1;
    }

    printf("Locale set successfully\n");

    // Restore original locale
    if (setlocale(LC_ALL, saved_locale) == NULL) {
        fprintf(stderr, "Failed to restore original locale\n");
        return 1;
    }

    return 0;
}