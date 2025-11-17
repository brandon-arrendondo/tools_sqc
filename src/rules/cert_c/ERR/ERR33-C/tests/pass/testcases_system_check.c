/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: PASS
 * Reason: system() return value is properly checked for execution errors
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    int result = system("echo 'Hello from system call'");

    if (result == -1) {
        fprintf(stderr, "Failed to execute system command\n");
        return 1;
    }

    if (result != 0) {
        fprintf(stderr, "System command failed with exit code: %d\n", result);
        return 1;
    }

    printf("System command executed successfully\n");
    return 0;
}