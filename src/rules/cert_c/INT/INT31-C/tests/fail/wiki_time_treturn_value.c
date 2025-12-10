/*
 * Rule: INT31-C
 * Source: wiki
 * Status: FAIL - Should trigger INT31-C violation
 * Description: time_t compared with -1 without proper cast
 */

#include <time.h>

void testcase_time_t_compare_no_cast(void) {
    time_t now = time(NULL);
    if (now != -1) {  /* Violation: -1 should be cast to time_t */
        /* Continue processing */
    }
}
