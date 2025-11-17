/*
 * Rule: ERR30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ERR30-C violation
 */

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
 
void func(const char *c_str) {
  unsigned long number;
  char *endptr;
 
  errno = 0;
  number = strtoul(c_str, &endptr, 0);
  if (endptr == c_str || (number == ULONG_MAX 
                         && errno == ERANGE)) {
    /* Handle error */
  } else {
    /* Computation succeeded */
  }
}