/*
 * Rule: PRE12-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE12-C violation
 */

m = (((++n) < 0) ? -(++n) : (++n));