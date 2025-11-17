/*
 * Rule: STR31-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: PASS
 * Reason: Array is explicitly sized to include space for null terminator
 */

#include <stdio.h>

int main() {
    char message[14] = "Hello, World!";  // 13 characters + 1 for null terminator
    char buffer[20];

    // Copy with proper size checking
    int i;
    for (i = 0; i < 13 && i < sizeof(buffer) - 1; i++) {
        buffer[i] = message[i];
    }
    buffer[i] = '\0';

    printf("Message: %s\n", buffer);

    return 0;
}