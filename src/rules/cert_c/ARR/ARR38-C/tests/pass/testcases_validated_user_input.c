/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: PASS
 * Reason: Validating user-controlled size before use
 */

#include <string.h>
#include <stdlib.h>

void process_data_safe(unsigned char *input, unsigned int user_size) {
    unsigned char buffer[256];

    // Validate user_size before use - COMPLIANT
    if (user_size > sizeof(buffer)) {
        user_size = sizeof(buffer);
    }

    memcpy(buffer, input, user_size);
}

int main(void) {
    unsigned char data[1000];
    // Size is now validated
    process_data_safe(data, 1000);
    return 0;
}
