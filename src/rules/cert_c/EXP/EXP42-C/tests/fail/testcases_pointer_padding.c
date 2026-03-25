/*
 * Rule: EXP42-C
 * Source: testcases
 * Status: FAIL - Do not compare padding data
 */

#include <string.h>

/* memcmp on structs with potential padding */
struct PaddedStruct {
    char a;
    int b;
};

int compare_structs(struct PaddedStruct *s1, struct PaddedStruct *s2) {
    return memcmp(s1, s2, sizeof(struct PaddedStruct));
}

/* memcmp on struct with mixed types */
struct MixedStruct {
    short x;
    double y;
};

int compare_mixed(struct MixedStruct *a, struct MixedStruct *b) {
    return memcmp(a, b, sizeof(struct MixedStruct));
}
