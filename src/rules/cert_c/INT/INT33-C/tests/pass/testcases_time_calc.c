/*
 * Rule: INT33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT33-C violation
 */

/*
 * Rule: INT33-C - Ensure that division and remainder operations do not result in divide-by-zero errors
 * Status: PASS
 * Reason: Time calculation validates divisor before converting seconds to hours/minutes
 */

#include <stdio.h>

void convert_seconds(int total_seconds) {
    if (total_seconds < 0) {
        printf("Error: Invalid negative time\n");
        return;
    }

    int hours = total_seconds / 3600;    // Safe: 3600 is never zero
    int minutes = (total_seconds % 3600) / 60;  // Safe: 60 is never zero
    int seconds = total_seconds % 60;    // Safe: 60 is never zero

    printf("%d seconds = %d hours, %d minutes, %d seconds\n",
           total_seconds, hours, minutes, seconds);
}

void calculate_speed(int distance, int time) {
    if (time == 0) {
        printf("Error: Time cannot be zero for speed calculation\n");
        return;
    }

    double speed = (double)distance / time;
    printf("Speed: %.2f units per time\n", speed);
}

int main() {
    convert_seconds(3661);
    calculate_speed(100, 5);
    return 0;
}