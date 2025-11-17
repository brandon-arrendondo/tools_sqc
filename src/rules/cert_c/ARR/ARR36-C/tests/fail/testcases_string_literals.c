/*
 * Rule: ARR36-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR36-C violation
 */

/*
 * Rule: ARR36-C - Do not subtract or compare two pointers that do not refer to the same array
 * Status: FAIL
 * Reason: Subtracting pointers to different string literals
 */

#include <stddef.h>

void string_diff(void) {
    const char *str1 = "Hello";
    const char *str2 = "World";

    // Subtract pointers from different string literals
    ptrdiff_t diff = str2 - str1;  // Line 14 - VIOLATION
}

int main(void) {
    string_diff();
    return 0;
}
