/*
 * Rule: ENV03-C
 * Source: testcases
 * Status: FAIL - External program invocation without environment sanitization
 */

#include <stdlib.h>

/* system() call without any environment sanitization */
void run_unsanitized_system(void) {
    system("ls -la /tmp");
}

/* popen() call without environment sanitization */
void run_unsanitized_popen(void) {
    popen("cat /etc/passwd", "r");
}

/* system() with unrelated function calls but no sanitization */
void system_with_no_sanitize(const char *cmd) {
    printf("Running: %s\n", cmd);
    system(cmd);
}

/* Multiple system() calls, none sanitized */
void multiple_unsanitized(void) {
    system("whoami");
    system("id");
}
