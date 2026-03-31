/*
 * Rule: INT12-C
 * Source: testcases
 * Status: FAIL - Should trigger INT12-C violation
 * Description: Plain int bit-fields have implementation-defined signedness
 */

struct flags {
    int enabled: 1;   /* Violation: plain int */
    int mode: 3;      /* Violation: plain int */
    int priority: 4;  /* Violation: plain int */
};

struct status {
    int active: 1;    /* Violation: plain int */
    int error_code: 8; /* Violation: plain int */
};
