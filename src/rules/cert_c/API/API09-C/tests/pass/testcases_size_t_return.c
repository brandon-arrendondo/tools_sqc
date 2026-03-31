/*
 * Rule: API09-C
 * Source: testcases
 * Status: PASS - Should NOT trigger API09-C violation
 *
 * Function uses size_t for accumulation with separate error indicator
 */

#include <unistd.h>

/* COMPLIANT: size_t return for accumulation, separate error handling */
size_t read_all(int fd, void *buf, size_t n, int *err) {
    size_t pos = 0;
    *err = 0;
    while (pos < n) {
        ssize_t res = read(fd, (char *)buf + pos, n - pos);
        if (res < 0) { *err = -1; return pos; }
        if (res == 0) break;
        pos += (size_t)res;
    }
    return pos;
}
