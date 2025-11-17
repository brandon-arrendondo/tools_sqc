/*
 * Rule: EXP45-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP45-C violation
 */

do { /* ... */ } while (foo(), x = y);