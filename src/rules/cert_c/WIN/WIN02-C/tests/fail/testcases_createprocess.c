/*
 * Rule: WIN02-C
 * Source: testcases
 * Status: FAIL - Should trigger WIN02-C violation
 *
 * Using CreateProcess() without restricted token
 */

#include <windows.h>

void launch_process(void) {
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    /* VIOLATION: CreateProcess without restricted user token */
    CreateProcess(NULL, "child.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}
