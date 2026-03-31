/*
 * Rule: API10-C
 * Source: testcases
 * Status: FAIL - Should trigger API10-C violation
 *
 * Security options require opt-in (insecure by default)
 */

/* VIOLATION: security is opt-in, meaning insecure by default */
#define VALIDATE_CERTIFICATES 0x0001
#define ENABLE_SECURITY 0x0002
