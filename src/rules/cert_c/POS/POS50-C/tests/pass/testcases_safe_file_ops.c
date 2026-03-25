/*
 * Rule: POS50-C
 * Source: testcases
 * Status: PASS - File operations without TOCTOU gap
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/* Open directly with fopen — no check-use gap */
void direct_open(const char *path) {
    FILE *f = fopen(path, "r");
    if (f) {
        fclose(f);
    }
}

/* Open with O_CREAT|O_EXCL — atomic create */
int atomic_create(const char *path) {
    int fd = open(path, O_CREAT | O_EXCL | O_WRONLY, 0600);
    if (fd >= 0) close(fd);
    return fd;
}
