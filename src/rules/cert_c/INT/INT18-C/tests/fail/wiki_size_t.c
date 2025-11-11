/*
 * Rule: INT18-C
 * Source: wiki
 * Status: FAIL - Should trigger INT18-C violation
 */

#include <stdlib.h>
 
void func(wchar_t *pwcs, const char *restrict s, size_t n) {
  size_t count_modified = mbstowcs(pwcs, s, n);
  if (count_modified == -1) {
    /* Handle error */
  }
}