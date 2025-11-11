/*
 * Rule: ARR01-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR01-C violation
 */

#include <stdio.h>
#include <string.h>

void safe_string_copy(char dest[], size_t dest_size, const char src[]) {
    if (dest_size > 0) {
        strncpy(dest, src, dest_size - 1);
        dest[dest_size - 1] = '\0';
    }
}

size_t safe_string_length(const char str[], size_t max_length) {
    size_t len = 0;
    while (len < max_length && str[len] != '\0') {
        len++;
    }
    return len;
}

int main() {
    char buffer[100];
    char source[] = "Hello, World!";
    size_t buffer_size = sizeof(buffer);
    size_t source_length = sizeof(source) - 1;

    printf("Buffer size: %zu bytes\n", buffer_size);
    printf("Source length: %zu characters\n", source_length);

    safe_string_copy(buffer, buffer_size, source);

    size_t actual_length = safe_string_length(buffer, buffer_size);
    printf("Copied string: %s (length: %zu)\n", buffer, actual_length);

    return 0;
}