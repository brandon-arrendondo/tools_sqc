/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM02-C violation
 */

widget *p;

/* ... */

p = (widget *)malloc(sizeof(widget));