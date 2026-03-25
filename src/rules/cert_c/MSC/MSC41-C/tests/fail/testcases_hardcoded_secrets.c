/*
 * Rule: MSC41-C
 * Source: testcases
 * Status: FAIL - Hard-coded sensitive data in source
 */

#include <string.h>

/* Hard-coded password string */
void hardcoded_password(void) {
    char *password_str = "secret123";
    (void)password_str;
}

/* Password in array initializer */
void password_array(void) {
    char pwd[] = "admin1234";
    (void)pwd;
}

/* Hard-coded key */
void hardcoded_key(void) {
    char *api_key = "AKIAIOSFODNN7EXAMPLE";
    (void)api_key;
}

/* Token in string */
void hardcoded_token(void) {
    char *auth_token = "Bearer eyJhbGciOiJIUzI1NiJ9";
    (void)auth_token;
}
