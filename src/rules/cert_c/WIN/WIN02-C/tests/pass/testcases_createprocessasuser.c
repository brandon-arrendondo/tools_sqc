/*
 * Rule: WIN02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger WIN02-C violation
 *
 * Using CreateProcessAsUser() with explicit token
 */

#include <windows.h>

void launch_process_safe(HANDLE token) {
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    /* COMPLIANT: explicit user token via CreateProcessAsUser */
    CreateProcessAsUser(token, NULL, "child.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}
