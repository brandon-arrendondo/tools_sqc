/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: strftime() return value is not checked for failure (0)
 */

#include <stdio.h>
#include <time.h>

int main() {
    time_t current_time = time(NULL);
    if (current_time == (time_t)(-1)) {
        return 1;
    }

    struct tm *timeinfo = localtime(&current_time);
    if (timeinfo == NULL) {
        return 1;
    }

    char buffer[50];
    // VIOLATION: Return value not checked for 0 (failure)
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Using buffer assuming formatting succeeded
    printf("Formatted time: %s\n", buffer); // May be uninitialized on error

    // Another unchecked strftime with too small buffer
    char small_buffer[5];
    strftime(small_buffer, sizeof(small_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    printf("Small buffer: %s\n", small_buffer);

    return 0;
}