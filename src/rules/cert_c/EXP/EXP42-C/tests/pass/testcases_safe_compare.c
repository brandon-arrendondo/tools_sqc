/*
 * Rule: EXP42-C
 * Source: testcases
 * Status: PASS - Safe comparison patterns
 */

#include <string.h>

/* Field-by-field comparison */
struct PaddedStruct {
    char a;
    int b;
};

int compare_fields(struct PaddedStruct *s1, struct PaddedStruct *s2) {
    return s1->a == s2->a && s1->b == s2->b;
}

/* memcmp on arrays (no padding) */
int compare_arrays(int *a, int *b, int n) {
    return memcmp(a, b, n * sizeof(int));
}

/* No memcmp usage */
int compare_ints(int a, int b) {
    return a == b;
}
