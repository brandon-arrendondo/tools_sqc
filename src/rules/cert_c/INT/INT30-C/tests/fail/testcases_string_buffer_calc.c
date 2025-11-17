/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Addition for string buffer size without wrap check
 */

#include <stdlib.h>
#include <string.h>

void concatenate_strings(const char *str1, const char *str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);

    // Addition may wrap
    size_t total_len = len1 + len2 + 1;  // Line 14 - VIOLATION

    char *result = malloc(total_len);
    if (result) {
        free(result);
    }
}

int main(void) {
    char large[SIZE_MAX / 2];
    concatenate_strings(large, large);
    return 0;
}
