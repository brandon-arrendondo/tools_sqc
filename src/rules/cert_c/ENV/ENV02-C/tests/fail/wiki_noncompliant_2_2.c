/*
 * Rule: ENV02-C
 * Source: wiki
 * Status: FAIL - Should trigger ENV02-C violation
 *
 * Demonstrates setting environment variables with names differing only in case
 */

#include <stdlib.h>

void setup_environment(void) {
    /* First set PATH in uppercase */
    putenv("PATH=/usr/bin:/bin");

    /* VIOLATION: Setting 'Path' differs only in case from 'PATH' */
    putenv("Path=/usr/local/bin");  /* Case-insensitive duplicate */
}

int main(void) {
    setup_environment();
    return 0;
}
