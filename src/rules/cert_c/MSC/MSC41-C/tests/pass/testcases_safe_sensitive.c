/*
 * Rule: MSC41-C
 * Source: testcases
 * Status: PASS - Non-hardcoded sensitive data
 */

#include <stdlib.h>

/* Password from environment variable */
const char *get_password(void) {
    return getenv("APP_PASSWORD");
}

/* Non-sensitive string literal */
void greet(void) {
    const char *msg = "Hello, World!";
    (void)msg;
}
