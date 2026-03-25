/*
 * Rule: MSC41-C
 * Source: testcases
 * Status: FAIL - Hardcoded sensitive data (passwords, keys)
 */

/* Hardcoded password */
void authenticate(void) {
    char *password = "secretpass123";
    (void)password;
}

/* Hardcoded API key */
void connect(void) {
    const char *api_key = "sk_live_1234567890abcdef";
    (void)api_key;
}
