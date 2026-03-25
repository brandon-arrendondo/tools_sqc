/*
 * Rule: API07-C
 * Source: testcases
 * Status: FAIL - strncpy usage without null-termination guarantee
 */

#include <string.h>

/* Basic strncpy call */
void basic_strncpy(char *dest, const char *src) {
    strncpy(dest, src, 100);
}

/* strncpy in a function with buffer */
void strncpy_with_buffer(const char *input) {
    char buffer[256];
    strncpy(buffer, input, sizeof(buffer));
}

/* strncpy with computed size */
void strncpy_computed(char *dest, const char *src, int len) {
    strncpy(dest, src, len);
}
