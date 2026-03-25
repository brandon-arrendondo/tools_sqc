/*
 * Rule: DCL13-C
 * Source: testcases
 * Status: PASS - Parameter aliased and modified, or passed to modifying function
 */

#include <stdlib.h>
#include <string.h>

/* Alias used to modify — should NOT suggest const */
void modify_via_alias(int *data, int n) {
    int *cur = data;
    for (int i = 0; i < n; i++) {
        *cur = 0;
        cur++;
    }
}

/* Passed to modifying function (memset) — should NOT suggest const */
void clear_buffer(char *buf, int size) {
    memset(buf, 0, size);
}

/* Passed to read-only function (strlen) — could be const but function parameter passing counts */
void log_length(const char *msg) {
    int len = strlen(msg);
    (void)len;
}
