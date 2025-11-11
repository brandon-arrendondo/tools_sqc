/*
 * Rule: STR37-C
 * Source: wiki
 * Status: PASS - Should NOT trigger STR37-C violation
 */

#include <ctype.h>
#include <string.h>
 
size_t count_preceding_whitespace(const char *s) {
  const char *t = s;
  size_t length = strlen(s) + 1;
  while (isspace((unsigned char)*t) && (t - s < length)) { 
    ++t;
  }
  return t - s;
}