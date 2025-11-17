/*
 * Rule: ARR02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR02-C violation
 */

#include <stdio.h>

static int global_buffer[1024] = {0};

static const double pi_digits[10] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};

static char error_messages[5][100] = {
    "No error",
    "Invalid input",
    "Memory allocation failed",
    "File not found"
};

void function_with_static_arrays(void) {
    static int counters[16] = {0};
    static char temp_buffer[512] = {0};

    static int fibonacci[20] = {0, 1, 1, 2, 3, 5, 8, 13};

    printf("Static arrays with explicit bounds\n");
}

int main() {
    function_with_static_arrays();
    return 0;
}