/*
 * Rule: PRE32-C
 * Source: testcases
 * Status: PASS - Should NOT trigger PRE32-C violation
 */

/*
 * Rule: PRE32-C - Do not use preprocessor directives in invocations of function-like macros
 * Status: PASS
 * Reason: String constant defined outside strcmp() call
 */

#include <string.h>

#if defined(PLATFORM_A)
#define PLATFORM_STRING "platform_a"
#elif defined(PLATFORM_B)
#define PLATFORM_STRING "platform_b"
#else
#define PLATFORM_STRING "default"
#endif

void compare_strings(const char *s1) {
    // Compliant: PLATFORM_STRING resolved before strcmp
    int result = strcmp(s1, PLATFORM_STRING);
}

int main(void) {
    compare_strings("test");
    return 0;
}
