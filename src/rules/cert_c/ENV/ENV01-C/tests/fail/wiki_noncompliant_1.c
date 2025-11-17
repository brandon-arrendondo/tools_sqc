/*
 * Rule: ENV01-C
 * Source: wiki
 * Status: FAIL - Should trigger ENV01-C violation
 */

void f() {
  char path[PATH_MAX]; /* Requires PATH_MAX to be defined */
  strcpy(path, getenv("PATH"));
  /* Use path */
}