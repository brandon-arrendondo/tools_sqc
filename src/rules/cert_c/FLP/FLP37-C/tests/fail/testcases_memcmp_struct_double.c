/*
 * Rule: FLP37-C
 * Source: testcases
 * Status: FAIL - Should trigger FLP37-C violation
 *
 * memcmp() on struct containing double field
 */

#include <string.h>

struct Measurement {
    int id;
    double value;
    int flags;
};

int compare_measurements(struct Measurement *a, struct Measurement *b) {
    /* VIOLATION: memcmp on struct with double member */
    return memcmp(a, b, sizeof(struct Measurement));
}
