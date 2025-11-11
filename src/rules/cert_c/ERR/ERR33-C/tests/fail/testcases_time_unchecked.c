/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: FAIL
 * Reason: time() return value is not checked for failure (-1)
 */

#include <stdio.h>
#include <time.h>

int main() {
    // VIOLATION: Return value not checked for (time_t)(-1)
    time_t current_time = time(NULL);

    // Using time value assuming call succeeded
    printf("Current time: %ld\n", (long)current_time); // May be -1 on error

    // Another unchecked time call
    time_t another_time;
    time(&another_time);
    printf("Another time: %ld\n", (long)another_time);

    return 0;
}