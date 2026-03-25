/*
 * Rule: STR31-C
 * Source: testcases
 * Status: PASS - Short string literals in strcat should not flag
 * Regression: Round 10 fix — strcat(data, "*.*") was incorrectly flagged
 */

#include <string.h>

void append_short_literals(char *data) {
    strcat(data, ".");
    strcat(data, "*.*");
    strcat(data, "\\");
    strcat(data, "/");
    wcscat((wchar_t *)data, L"*.*");
}
