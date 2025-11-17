/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: mktime() return value is not checked for failure (-1)
 */

#include <stdio.h>
#include <time.h>

int main() {
    struct tm timeinfo = {0};
    timeinfo.tm_year = 121; // 2021
    timeinfo.tm_mon = 11;   // December
    timeinfo.tm_mday = 25;  // 25th

    // VIOLATION: Return value not checked for (time_t)(-1)
    time_t timestamp = mktime(&timeinfo);

    // Using timestamp assuming conversion succeeded
    printf("Timestamp: %ld\n", (long)timestamp); // May be -1 on error

    // Another unchecked mktime with invalid date
    timeinfo.tm_mon = 13; // Invalid month
    timestamp = mktime(&timeinfo);
    printf("Invalid timestamp: %ld\n", (long)timestamp);

    return 0;
}