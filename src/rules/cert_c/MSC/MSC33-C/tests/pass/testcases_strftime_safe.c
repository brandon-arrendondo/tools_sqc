/*
 * Rule: MSC33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger MSC33-C violation
 *
 * strftime() is a safe alternative to asctime()
 */

#include <stdio.h>
#include <time.h>

void print_current_time(void) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char buffer[64];
    /* COMPLIANT: strftime with explicit buffer size */
    strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y\n", tm_info);
    printf("Current time: %s", buffer);
}
