/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: FAIL
 * Reason: Environment variable used as format string
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
    char *env_format = getenv("LOG_FORMAT");

    if (env_format) {
        // VULNERABLE: environment variable as format string
        printf(env_format);
    } else {
        printf("LOG_FORMAT not set\n");
    }

    return 0;
}