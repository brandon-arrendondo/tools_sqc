/*
 * Rule: INT00-C
 * Source: wiki
 * Status: FAIL - Should trigger INT00-C violation
 */

unsigned int a, b;
unsigned long c;
/* Initialize a and b */
c = (unsigned long)a * b; /* Not guaranteed to fit */