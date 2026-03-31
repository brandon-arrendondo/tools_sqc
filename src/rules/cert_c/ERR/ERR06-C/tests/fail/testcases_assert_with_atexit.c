/*
 * Rule: ERR06-C
 * Source: testcases
 * Status: FAIL - Should trigger ERR06-C violation
 * Description: assert() used when atexit cleanup handlers are registered
 */

#include <stdlib.h>
#include <assert.h>

void cleanup_temp_files(void) {
    /* remove temp files */
}

void process(int *data, int len) {
    atexit(cleanup_temp_files);

    assert(data != NULL);  /* Violation: abort() bypasses atexit */
    assert(len > 0);       /* Violation: abort() bypasses atexit */

    for (int i = 0; i < len; i++) {
        data[i] *= 2;
    }
}
