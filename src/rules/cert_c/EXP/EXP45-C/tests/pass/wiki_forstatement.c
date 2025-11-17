/*
 * Rule: EXP45-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP45-C violation
 */

for (; x; foo(), x = y) { /* ... */ }