/*
 * Rule: DCL30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL30-C violation
 */

const char *p;
void is_this_OK(void) {
  const char c_str[] = "Everything OK?";
  p = c_str;
  /* ... */
  p = NULL;
}