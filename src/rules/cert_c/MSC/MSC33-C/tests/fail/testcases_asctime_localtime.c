/*
 * Rule: MSC33-C
 * Source: testcases
 * Status: FAIL - Should trigger MSC33-C violation
 *
 * asctime() with localtime() is fundamentally unsafe
 */

#include <stdio.h>
#include <time.h>

void print_current_time(void) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    /* VIOLATION: asctime can overflow with invalid tm data */
    char *time_str = asctime(tm_info);
    printf("Current time: %s", time_str);
}
