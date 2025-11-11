/*
 * Rule: ARR02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR02-C violation
 */

#include <stdio.h>

#define BUFFER_SIZE 4096
#define MAX_CONNECTIONS 1000

int main() {
    char large_buffer[BUFFER_SIZE] = {0};

    int connection_ids[MAX_CONNECTIONS] = {[0] = 1, [999] = 1000};

    unsigned char bitmap[512] = {0xFF, 0x00, 0xFF};

    short temperature_readings[365] = {
        [0] = 20,     // Jan 1
        [31] = 18,    // Feb 1
        [364] = 22    // Dec 31
    };

    long long timestamps[100] = {0};

    printf("Large arrays with explicit bounds\n");

    return 0;
}