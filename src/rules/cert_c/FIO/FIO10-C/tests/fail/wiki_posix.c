/*
 * Rule: FIO10-C
 * Source: wiki
 * Status: FAIL - rename() without any error handling or destination check
 */

#include <stdio.h>

void unsafe_rename(const char *src, const char *dst) {
    rename(src, dst);
}