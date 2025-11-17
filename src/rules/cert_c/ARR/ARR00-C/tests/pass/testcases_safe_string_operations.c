/*
 * Rule: ARR00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR00-C violation
 */

#include <stdio.h>
#include <string.h>

int main() {
    char buffer[100];
    const char *source = "Hello, World!";

    strncpy(buffer, source, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char dest[50];
    size_t source_len = strlen(source);
    if (source_len < sizeof(dest)) {
        strcpy(dest, source);
        printf("String copied: %s\n", dest);
    } else {
        printf("Source string too large for destination buffer\n");
    }

    char concat_buffer[200] = "Start: ";
    size_t remaining = sizeof(concat_buffer) - strlen(concat_buffer) - 1;
    if (strlen(source) <= remaining) {
        strncat(concat_buffer, source, remaining);
        printf("Concatenated: %s\n", concat_buffer);
    }

    return 0;
}