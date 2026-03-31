/*
 * Rule: API10-C
 * Source: testcases
 * Status: PASS - Should NOT trigger API10-C violation
 *
 * Security options are secure by default, opt-out to weaken
 */

/* COMPLIANT: secure by default, must explicitly disable */
#define DISABLE_CERTIFICATE_VALIDATION 0x0001
#define ALLOW_INSECURE_CONNECTIONS 0x0002
