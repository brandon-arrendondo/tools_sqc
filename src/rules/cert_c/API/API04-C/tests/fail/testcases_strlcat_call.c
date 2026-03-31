/*
 * Rule: API04-C
 * Source: testcases
 * Status: FAIL - Should trigger API04-C violation
 *
 * strlcat() has inconsistent error-checking semantics
 */

#include <string.h>

void build_path(char *result, size_t size,
                const char *dir, const char *file) {
    /* VIOLATION: strlcat returns input length, awkward error checking */
    strlcpy(result, dir, size);
    strlcat(result, "/", size);
    strlcat(result, file, size);
}
