/*
 * Rule: STR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: PASS
 * Reason: Using const pointer, only reading result
 */

#include <string.h>

void find_char(void) {
    // Compliant: using const pointer, not modifying
    const char *ptr = strchr("Hello World", 'W');
    if (ptr) {
        // Only reading, not modifying
        char found = *ptr;
    }
}

int main(void) {
    find_char();
    return 0;
}
