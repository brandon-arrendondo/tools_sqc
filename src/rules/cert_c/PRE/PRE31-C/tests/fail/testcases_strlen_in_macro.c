/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: strlen (may modify errno) in unsafe macro
 */

#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))  /* UNSAFE */

void string_compare(const char *s1, const char *s2) {
    // strlen may have side effects (errno) - evaluated twice
    size_t max_len = MAX(strlen(s1), strlen(s2));  // Line 13 - VIOLATION
}

int main(void) {
    string_compare("hello", "world");
    return 0;
}
