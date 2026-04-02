/*
 * Rule: FLP37-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FLP37-C violation
 *
 * memcmp() on struct with only integer fields
 */

#include <string.h>

struct IntRecord {
    int id;
    int count;
    unsigned flags;
};

int compare_records(struct IntRecord *a, struct IntRecord *b) {
    /* COMPLIANT: no floating-point fields in struct */
    return memcmp(a, b, sizeof(struct IntRecord));
}
