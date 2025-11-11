/*
 * Rule: ARR02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR02-C violation
 */

#include <stdio.h>

enum color {
    RED, GREEN, BLUE, YELLOW, BLACK, WHITE, COLOR_COUNT
};

enum status {
    IDLE, RUNNING, PAUSED, STOPPED, STATUS_COUNT
};

int main() {
    const char* color_names[COLOR_COUNT] = {
        [RED] = "Red",
        [GREEN] = "Green",
        [BLUE] = "Blue",
        [YELLOW] = "Yellow",
        [BLACK] = "Black",
        [WHITE] = "White"
    };

    int status_priorities[STATUS_COUNT] = {
        [IDLE] = 1,
        [RUNNING] = 3,
        [PAUSED] = 2,
        [STOPPED] = 0
    };

    double color_wavelengths[COLOR_COUNT] = {
        [RED] = 700.0,
        [GREEN] = 530.0,
        [BLUE] = 470.0
    };

    printf("Enum-based arrays with explicit bounds\n");

    return 0;
}