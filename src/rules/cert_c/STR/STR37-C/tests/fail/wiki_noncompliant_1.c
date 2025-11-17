/*
 * Rule: STR37-C
 * Source: wiki
 * Status: FAIL - Should trigger STR37-C violation
 */

#include <ctype.h>
#include <string.h>
 
size_t count_preceding_whitespace(const char *s) {
  const char *t = s;
  size_t length = strlen(s) + 1;
  while (isspace(*t) && (t - s < length)) { 
    ++t;
  }
  return t - s;
}