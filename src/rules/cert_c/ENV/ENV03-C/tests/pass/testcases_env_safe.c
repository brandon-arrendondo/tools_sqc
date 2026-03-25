/*
 * Rule: ENV03-C
 * Source: testcases
 * Status: PASS - Environment sanitized before invoking external programs
 */

#include <stdlib.h>

/* clearenv() before system() */
void sanitized_with_clearenv(void) {
    clearenv();
    system("ls -la /tmp");
}

/* clearenv() plus setenv before system() */
void sanitized_full(void) {
    clearenv();
    setenv("PATH", "/usr/bin:/bin", 1);
    setenv("IFS", " \t\n", 1);
    system("ls /home");
}

/* clearenv() before popen() */
void sanitized_popen(void) {
    clearenv();
    popen("ls", "r");
}

/* No system() or popen() calls at all */
void no_external_invocation(void) {
    int x = 42;
    (void)x;
}

/* Function with only printf, no system/popen */
void safe_logging(void) {
    printf("No external commands here\n");
}
