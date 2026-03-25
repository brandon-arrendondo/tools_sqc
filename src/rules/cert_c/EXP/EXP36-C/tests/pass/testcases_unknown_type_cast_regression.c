/*
 * Rule: EXP36-C
 * Source: testcases
 * Status: PASS - Unknown source types should not be flagged for alignment issues
 * Regression: Round 7 fix — only flag casts with confirmed non-aligned source types
 */

#include <stdlib.h>

struct aligned_struct {
    int x;
    double y;
};

void test_unknown_source_casts(void) {
    /* Cast from malloc (returns void*) — source type unknown, should not flag */
    struct aligned_struct *p = (struct aligned_struct *)malloc(sizeof(struct aligned_struct));
    free(p);

    /* Cast from function call returning unknown type */
    void *raw = malloc(100);
    int *ip = (int *)raw;
    (void)ip;
    free(raw);
}
