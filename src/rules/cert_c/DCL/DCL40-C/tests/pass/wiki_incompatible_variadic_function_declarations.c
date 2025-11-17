/*
 * Rule: DCL40-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL40-C violation
 */

/* In a.c */
void buginf(const char *fmt, ...) {
   /* ... */
}

/* In b.c */
void buginf(const char *fmt, ...);