/*
 * Rule: STR03-C
 * Source: testcases
 * Status: PASS - strncpy preceded by strlen validation
 */

#include <string.h>

#define BUFFER_SIZE 64

/* strlen check in preceding if, strncpy after */
void strncpy_after_strlen_check(const char *src) {
    char dest[BUFFER_SIZE];
    if (strlen(src) >= sizeof(dest)) {
        return;
    }
    strncpy(dest, src, sizeof(dest));
    dest[BUFFER_SIZE - 1] = '\0';
}

/* strlen check with > comparison */
void strncpy_after_strlen_gt(const char *src) {
    char dest[BUFFER_SIZE];
    if (strlen(src) > BUFFER_SIZE - 1) {
        return;
    }
    strncpy(dest, src, BUFFER_SIZE);
    dest[BUFFER_SIZE - 1] = '\0';
}
