/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: PASS
 * Reason: Using strlen directly without sizeof multiplication
 */

#include <string.h>

void append_string(void) {
    char message[100] = "Hello";
    char *append_pos;

    // Use strlen directly - COMPLIANT
    append_pos = message + strlen(message);
    strcpy(append_pos, " World");
}

int main(void) {
    append_string();
    return 0;
}
