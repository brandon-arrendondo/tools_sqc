/*
 * Rule: ERR33-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ERR33-C violation
 */

/*
 * Rule: ERR33-C - Detect and handle standard library errors
 * Status: PASS
 * Reason: getenv() return value is properly checked for NULL before use
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char *env_var = "PATH";
    char *path = getenv(env_var);

    if (path == NULL) {
        fprintf(stderr, "Environment variable %s not found\n", env_var);
        return 1;
    }

    printf("PATH environment variable found\n");
    printf("Length: %zu characters\n", strlen(path));

    // Check for HOME environment variable
    char *home = getenv("HOME");
    if (home == NULL) {
        printf("HOME environment variable not set\n");
    } else {
        printf("HOME directory: %.50s...\n", home);
    }

    return 0;
}