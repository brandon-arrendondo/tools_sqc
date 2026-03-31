/*
 * Rule: API09-C
 * Source: testcases
 * Status: FAIL - Should trigger API09-C violation
 *
 * Function returns ssize_t while accumulating byte count
 */

#include <unistd.h>

/* VIOLATION: ssize_t return accumulates size values */
ssize_t read_all(int fd, void *buf, size_t n) {
    ssize_t pos = 0;
    while ((size_t)pos < n) {
        ssize_t res = read(fd, (char *)buf + pos, n - pos);
        if (res < 0) return -1;
        if (res == 0) break;
        pos += res;
    }
    return pos;
}
