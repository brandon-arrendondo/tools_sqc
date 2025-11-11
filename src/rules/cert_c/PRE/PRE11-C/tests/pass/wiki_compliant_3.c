/*
 * Rule: PRE11-C
 * Source: wiki
 * Status: PASS - Should NOT trigger PRE11-C violation
 */

inline int incremod(int *x, int max) {*x = (*x + 1) % max;}