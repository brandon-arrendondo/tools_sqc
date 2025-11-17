/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Environment variable value might exceed buffer size
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *env_value;
    char buffer[20];

    env_value = getenv("PATH");  // PATH can be very long
    if (env_value) {
        strcpy(buffer, env_value);  // Environment variable might be > 20 chars
        printf("PATH: %s\n", buffer);
    }

    return 0;
}