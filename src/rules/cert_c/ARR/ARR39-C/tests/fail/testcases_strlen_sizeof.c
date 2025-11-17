/*
 * Rule: ARR39-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR39-C violation
 */

/*
 * Rule: ARR39-C - Do not add or subtract a scaled integer to a pointer
 * Status: FAIL
 * Reason: Multiplying strlen by sizeof(char) unnecessarily
 */

#include <string.h>

void strlen_scaled(void) {
    char message[100] = "Hello";
    char *append_pos;

    // Multiplying strlen by sizeof(char) - technically 1, but wrong pattern
    append_pos = message + (strlen(message) * sizeof(char));  // Line 13 - VIOLATION
    strcpy(append_pos, " World");
}

int main(void) {
    strlen_scaled();
    return 0;
}
