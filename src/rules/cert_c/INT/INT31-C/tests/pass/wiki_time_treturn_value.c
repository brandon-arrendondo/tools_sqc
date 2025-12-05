/*
 * Rule: INT31-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT31-C violation
 * Description: time_t compared with properly cast -1
 */

#include <time.h>

void testcase_time_t_compare_with_cast(void) {
    time_t now = time(NULL);
    if (now != (time_t)-1) {  /* Compliant: -1 properly cast */
        /* Continue processing */
    }
}
