/*
 * Rule: MEM35-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM35-C violation
 */

#include <stdint.h>
#include <stdlib.h>
 
void function(size_t len) {
  long *p;
  if (len == 0 || len > SIZE_MAX / sizeof(*p)) {
    /* Handle overflow */
  }
  p = (long *)malloc(len * sizeof(*p));
  if (p == NULL) {
    /* Handle error */
  }
  free(p);
}