/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: setlocale() return value is not checked for failure
 */

#include <stdio.h>
#include <locale.h>

int main() {
    // VIOLATION: Return value not checked
    setlocale(LC_ALL, "invalid_locale");

    // Continue execution assuming locale was set successfully
    printf("Locale supposedly set\n");

    // Another unchecked setlocale call
    setlocale(LC_NUMERIC, "C");
    printf("Numeric locale supposedly set\n");

    return 0;
}