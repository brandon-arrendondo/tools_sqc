/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Modifying result from strchr() when input is string literal
 */

#include <string.h>

void replace_char(void) {
    char *ptr = strchr("Hello World", 'W');  // Line 10 - VIOLATION: treating result as modifiable
    if (ptr) {
        *ptr = 'w';  // Line 12 - VIOLATION: modifying string literal
    }
}

int main(void) {
    replace_char();
    return 0;
}
