/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: system() return value is not checked for execution errors
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    // VIOLATION: Return value not checked
    system("nonexistent_command_xyz");

    printf("Command supposedly executed\n");

    // Another unchecked system call
    system("rm -f /nonexistent/path/file.txt");
    printf("File supposedly removed\n");

    return 0;
}