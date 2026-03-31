/*
 * Rule: WIN01-C
 * Source: testcases
 * Status: FAIL - Should trigger WIN01-C violation
 *
 * Using TerminateThread() for thread termination
 */

#include <windows.h>

void kill_thread(HANDLE thread) {
    /* VIOLATION: TerminateThread does not allow cleanup */
    TerminateThread(thread, 0);
}
