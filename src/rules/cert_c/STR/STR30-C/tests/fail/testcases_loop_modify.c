/*
 * Rule: STR30-C
 * Source: testcases
 * Status: FAIL - Should trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: FAIL
 * Reason: Modifying string literal in loop
 */

#include <string.h>

void reverse_string(void) {
    char *str = "reverse";  // Line 10 - VIOLATION: non-const pointer to string literal
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];  // Line 14 - VIOLATION: modifying string literal
        str[len - i - 1] = temp;  // Line 15 - VIOLATION: modifying string literal
    }
}

int main(void) {
    reverse_string();
    return 0;
}
