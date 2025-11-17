/*
 * Rule: EXP13-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP13-C violation
 */

if ( (a < b) && (b < c) ) /* Clearer and probably what was intended */
/* ... */
if ( (a == b) && (a == c) ) /* Ditto */