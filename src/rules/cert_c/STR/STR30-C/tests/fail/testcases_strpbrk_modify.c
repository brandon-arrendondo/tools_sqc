/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Modifying result from strpbrk() when input is string literal
 */

#include <string.h>

void find_and_modify(void) {
    char *ptr = strpbrk("Hello World", "aeiou");  // Line 10 - VIOLATION: treating result as modifiable
    if (ptr) {
        *ptr = 'X';  // Line 12 - VIOLATION: modifying string literal
    }
}

int main(void) {
    find_and_modify();
    return 0;
}
