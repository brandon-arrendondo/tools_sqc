/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Modifying result from strrchr() when input is string literal
 */

#include <string.h>

const char *get_dirname(const char *pathname) {
    char *slash;
    slash = strrchr(pathname, '/');  // Line 11 - VIOLATION: treating result as modifiable
    if (slash) {
        *slash = '\0';  // Line 13 - VIOLATION: modifying string literal
    }
    return pathname;
}

int main(void) {
    get_dirname("/usr/local/bin");
    return 0;
}
