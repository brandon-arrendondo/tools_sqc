/*
 * Rule: PRE31-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE31-C violation
 */

m = (((++n) < 0) ? -(++n) : (++n));