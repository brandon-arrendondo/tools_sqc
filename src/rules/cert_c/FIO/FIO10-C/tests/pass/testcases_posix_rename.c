/*
 * Rule: FIO10-C
 * Source: testcases
 * Status: PASS - POSIX rename() with error checking is compliant.
 *         On POSIX, rename() atomically replaces the destination.
 */

#include <stdio.h>

void posix_rename_with_error_check(const char *src, const char *dst) {
    if (rename(src, dst) != 0) {
        perror("rename failed");
    }
}

void posix_rename_negated_check(const char *src, const char *dst) {
    if (!rename(src, dst)) {
        /* success */
    }
}
