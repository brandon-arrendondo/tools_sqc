/*
 * Rule: FIO38-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO38-C violation
 * Description: memcpy on non-FILE types is fine
 */

#include <string.h>

struct Data {
    int values[10];
    double total;
};

void copy_struct(struct Data *dst, const struct Data *src) {
    memcpy(dst, src, sizeof(struct Data));  /* Safe: not a FILE */
}

void copy_buffer(char *dst, const char *src, int n) {
    memcpy(dst, src, n);  /* Safe: not a FILE */
}
