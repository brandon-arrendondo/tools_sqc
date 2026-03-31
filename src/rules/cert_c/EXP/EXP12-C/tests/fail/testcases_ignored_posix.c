/*
 * Rule: EXP12-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP12-C violation
 * Description: Return values from POSIX functions ignored
 */

#include <signal.h>
#include <stdlib.h>

void cleanup(void) { /* no-op */ }

void ignore_posix_returns(void) {
    signal(2, cleanup);  /* Violation: return value ignored */
    system("ls");        /* Violation: return value ignored */
    atexit(cleanup);     /* Violation: return value ignored */
}
