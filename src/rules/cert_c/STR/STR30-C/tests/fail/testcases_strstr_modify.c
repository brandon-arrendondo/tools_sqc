/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Modifying result from strstr() when input is string literal
 */

#include <string.h>

void replace_substring(void) {
    char *ptr = strstr("Hello World", "World");  // Line 10 - VIOLATION: treating result as modifiable
    if (ptr) {
        *ptr = 'w';  // Line 12 - VIOLATION: modifying string literal
    }
}

int main(void) {
    replace_substring();
    return 0;
}
