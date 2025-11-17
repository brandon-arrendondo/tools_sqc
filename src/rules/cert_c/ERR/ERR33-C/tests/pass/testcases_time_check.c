/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: PASS
 * Reason: time() return value is properly checked for failure
 */

#include <stdio.h>
#include <time.h>

int main() {
    time_t current_time = time(NULL);
    if (current_time == (time_t)(-1)) {
        fprintf(stderr, "Failed to get current time\n");
        return 1;
    }

    printf("Current time: %ld\n", (long)current_time);

    // Convert to string representation
    char *time_str = ctime(&current_time);
    if (time_str == NULL) {
        fprintf(stderr, "Failed to convert time to string\n");
        return 1;
    }

    printf("Time string: %s", time_str);

    // Get local time structure
    struct tm *local_time = localtime(&current_time);
    if (local_time == NULL) {
        fprintf(stderr, "Failed to get local time structure\n");
        return 1;
    }

    printf("Year: %d, Month: %d, Day: %d\n",
           local_time->tm_year + 1900,
           local_time->tm_mon + 1,
           local_time->tm_mday);

    return 0;
}