/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: Using user-controlled size without validation (Heartbleed-style)
 */

#include <string.h>
#include <stdlib.h>

void process_data(unsigned char *input, unsigned int user_size) {
    unsigned char buffer[256];

    // No validation of user_size - could be larger than buffer
    memcpy(buffer, input, user_size);  // Line 13 - VIOLATION
}

int main(void) {
    unsigned char data[1000];
    // Attacker controls size parameter
    process_data(data, 1000);  // Overflows buffer
    return 0;
}
