/*
 * Rule: DCL40-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL40-C violation
 */

/* In a.c */
void buginf(const char *fmt, ...) {
   /* ... */
}
 
/* In b.c */
void buginf();