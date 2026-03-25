/*
 * Rule: MSC41-C
 * Source: testcases
 * Status: PASS - Secrets loaded dynamically (not hard-coded)
 */

#include <stdlib.h>
#include <stdio.h>

/* Password from environment variable */
void env_password(void) {
    char *password = getenv("PASSWORD");
    (void)password;
}

/* Empty password placeholder (not a real secret) */
void empty_password(void) {
    char password[256];
    password[0] = '\0';
    (void)password;
}

/* Non-sensitive string variable */
void regular_string(void) {
    char *msg = "Hello, World!";
    (void)msg;
}
