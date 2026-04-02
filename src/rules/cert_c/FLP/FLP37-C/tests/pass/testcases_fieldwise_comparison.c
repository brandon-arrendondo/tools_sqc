/*
 * Rule: FLP37-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FLP37-C violation
 *
 * Field-by-field comparison instead of memcmp
 */

#include <math.h>

struct Point {
    int id;
    float x;
    float y;
};

int compare_points(struct Point *a, struct Point *b) {
    /* COMPLIANT: field-by-field comparison, not memcmp */
    return (a->id == b->id) &&
           (fabsf(a->x - b->x) < 1e-6f) &&
           (fabsf(a->y - b->y) < 1e-6f);
}
