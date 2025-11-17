/*
 * Rule: MEM30-C
 * Source: testcases
 * Status: FAIL - Should trigger MEM30-C violation
 */

/*
 * Rule: MEM30-C - Do not access freed memory
 * Status: FAIL
 * Reason: Uses memcpy with freed source or destination buffer
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
    char *src = malloc(20);
    char *dst = malloc(20);

    if (src == NULL || dst == NULL) {
        free(src);
        free(dst);
        return -1;
    }

    strcpy(src, "Hello");

    free(src);

    // BUG: memcpy from freed memory
    memcpy(dst, src, 6);

    printf("Copied: %s\n", dst);

    free(dst);
    return 0;
}