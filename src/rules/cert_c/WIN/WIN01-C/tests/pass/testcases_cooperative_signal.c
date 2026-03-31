/*
 * Rule: WIN01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger WIN01-C violation
 *
 * Cooperative thread termination via signaling
 */

#include <windows.h>

volatile LONG shutdown_flag = 0;

void signal_shutdown(void) {
    /* COMPLIANT: cooperative signaling instead of TerminateThread */
    InterlockedExchange(&shutdown_flag, 1);
}
