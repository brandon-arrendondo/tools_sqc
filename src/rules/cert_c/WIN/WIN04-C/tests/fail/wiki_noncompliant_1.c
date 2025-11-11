/*
 * Rule: WIN04-C
 * Source: wiki
 * Status: FAIL - Should trigger WIN04-C violation
 */

int (*log_fn)(const char *, ...) = printf;
/* ... */
log_fn("foo");