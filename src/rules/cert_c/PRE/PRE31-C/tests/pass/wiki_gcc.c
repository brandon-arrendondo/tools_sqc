/*
 * Rule: PRE31-C
 * Source: wiki
 * Status: PASS - Should NOT trigger PRE31-C violation
 */

#define ABS(x) __extension__ ({ __typeof (x) tmp = x; \
                    tmp < 0 ? -tmp : tmp; })