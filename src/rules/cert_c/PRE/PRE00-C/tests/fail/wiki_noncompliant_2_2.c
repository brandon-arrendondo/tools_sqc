/*
 * Rule: PRE00-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE00-C violation
 */

int a = 81 / ((++i) * (++i) * (++i));