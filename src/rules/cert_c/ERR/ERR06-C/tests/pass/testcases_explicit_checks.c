/*
 * Rule: ERR06-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ERR06-C violation
 * Description: Explicit error checking instead of assert with atexit
 */

#include <stdlib.h>
#include <stdio.h>

void cleanup_resources(void) {
    /* cleanup logic */
}

int process(int *data, int len) {
    if (atexit(cleanup_resources) != 0) {
        return -1;
    }

    if (data == NULL || len <= 0) {
        fprintf(stderr, "Invalid arguments\n");
        exit(EXIT_FAILURE);  /* Safe: triggers atexit handlers */
    }

    for (int i = 0; i < len; i++) {
        data[i] *= 2;
    }
    return 0;
}
