/*
 * Rule: STR03-C
 * Source: testcases
 * Status: FAIL - Strings not properly null-terminated
 */

#include <string.h>

/* strncpy without explicit null termination */
void strncpy_no_term(char *dest, const char *src) {
    strncpy(dest, src, 10);
    /* Missing: dest[10] = '\0'; */
}

/* char array initialization without room for null */
void array_no_room(void) {
    char buf[5] = "hello"; /* Exact fit, no null terminator */
    (void)buf;
}

/* memcpy without null termination */
void memcpy_no_term(char *dest, const char *src, int len) {
    memcpy(dest, src, len);
    /* Not null-terminated */
}
