/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM02-C violation
 */

enum { N = 16 };
widget *p;

/* ... */

p = MALLOC_ARRAY(N, widget);    /* OK */