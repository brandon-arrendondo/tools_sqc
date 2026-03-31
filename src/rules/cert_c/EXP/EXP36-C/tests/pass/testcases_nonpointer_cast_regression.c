/*
 * Rule: EXP36-C
 * Source: testcases
 * Status: PASS - Non-pointer casts should not be flagged for alignment issues
 * Regression: Round 7 fix — only check pointer-to-pointer casts, skip integer casts
 *
 * EXP36-C is about pointer alignment. Casts to non-pointer types like
 * (unsigned), (int), (long) are integer conversions, not pointer alignment
 * issues, and must not trigger EXP36-C.
 */

#include <stdlib.h>
#include <time.h>

void test_integer_casts(void) {
    /* Cast time_t to unsigned — integer conversion, not pointer alignment */
    unsigned seed = (unsigned)time(NULL);
    (void)seed;

    /* Cast pointer to integer — not a pointer-to-pointer alignment issue */
    int *p = malloc(sizeof(int));
    long addr = (long)p;
    (void)addr;
    free(p);

    /* Cast between integer types */
    long big = 100000L;
    int small = (int)big;
    (void)small;

    /* Cast size_t result */
    char buf[64];
    int len = (int)sizeof(buf);
    (void)len;
}
