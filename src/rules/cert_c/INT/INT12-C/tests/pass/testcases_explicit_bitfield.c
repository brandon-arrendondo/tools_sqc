/*
 * Rule: INT12-C
 * Source: testcases
 * Status: PASS - Should NOT trigger INT12-C violation
 * Description: Explicit signedness on all bit-fields
 */

struct flags {
    unsigned int enabled: 1;
    unsigned int mode: 3;
    signed int priority: 4;
};

struct status {
    unsigned int active: 1;
    unsigned int error_code: 8;
    signed int offset: 7;
};
