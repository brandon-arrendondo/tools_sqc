/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: security_violations.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Password environment modification */
void unsafe_password_modification(void) {
    char *password = getenv("DATABASE_PASSWORD");
    if (password) {
        /* VIOLATION: Attempting to "mask" password */
        for (int i = 1; password[i]; i++) {
            password[i] = '*';  /* Undefined behavior */
        }
        printf("Masked password: %s\n", password);
    }
}

/* NON-COMPLIANT: Token modification */
void unsafe_token_modification(void) {
    char *token = getenv("API_TOKEN");
    if (token) {
        /* VIOLATION: Truncating token for display */
        if (strlen(token) > 10) {
            token[10] = '\0';  /* Undefined behavior */
        }
        printf("Truncated token: %s\n", token);
    }
}

int main(void) {
    setenv("DATABASE_PASSWORD", "secret123", 1);
    setenv("API_TOKEN", "abcdef1234567890", 1);

    unsafe_password_modification();
    unsafe_token_modification();
    return 0;
}