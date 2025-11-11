/*
 * Rule: PRE02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger PRE02-C violation
 */

enum { END_OF_FILE = -1 };
/* ... */
if (getchar() != END_OF_FILE) {
   /* ... */
}