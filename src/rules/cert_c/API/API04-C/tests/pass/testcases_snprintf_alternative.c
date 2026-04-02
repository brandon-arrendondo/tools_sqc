/*
 * Rule: API04-C
 * Source: testcases
 * Status: PASS - Should NOT trigger API04-C violation
 *
 * snprintf() provides consistent error-checking semantics
 */

#include <stdio.h>

void build_path(char *result, size_t size,
                const char *dir, const char *file) {
    /* COMPLIANT: snprintf returns characters written, consistent API */
    int ret = snprintf(result, size, "%s/%s", dir, file);
    if (ret < 0 || (size_t)ret >= size) {
        result[0] = '\0';
    }
}
