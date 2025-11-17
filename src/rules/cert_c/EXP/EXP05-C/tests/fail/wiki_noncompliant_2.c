/*
 * Rule: EXP05-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP05-C violation
 */

const int vals[3] = {3, 4, 5};
memset(vals, 0, sizeof(vals));