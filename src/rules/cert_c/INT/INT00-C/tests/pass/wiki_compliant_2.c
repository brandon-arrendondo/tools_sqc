/*
 * Rule: INT00-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT00-C violation
 */

#if UINT_MAX > UINTMAX_MAX/UINT_MAX
#error No safe type is available.
#endif
/* ... */
unsigned int a, b;
uintmax_t c;
/* Initialize a and b */
c = (uintmax_t)a * b; /* Guaranteed to fit, verified above */