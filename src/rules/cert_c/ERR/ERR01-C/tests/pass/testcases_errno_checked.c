/*
 * Rule: ERR01-C
 * Source: testcases
 * Status: PASS - errno properly checked after library call
 */

#include <errno.h>
#include <stdlib.h>

/* strtol with errno check */
long safe_parse(const char *str) {
    errno = 0;
    long result = strtol(str, NULL, 10);
    if (errno != 0) {
        return 0;
    }
    return result;
}
