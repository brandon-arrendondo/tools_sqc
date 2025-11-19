/*
 * Rule: ENV02-C
 * Source: wiki
 * Status: FAIL - Should trigger ENV02-C violation
 *
 * Demonstrates using setenv() with case-insensitive duplicate names
 */

#include <stdlib.h>

void configure_env(void) {
    /* Set HOME in uppercase */
    setenv("HOME", "/home/user", 1);

    /* VIOLATION: Setting 'home' differs only in case from 'HOME' */
    setenv("home", "/home/otheruser", 1);  /* Case-insensitive duplicate */
}

int main(void) {
    configure_env();
    return 0;
}
