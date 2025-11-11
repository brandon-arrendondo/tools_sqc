/*
 * Rule: FIO42-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO42-C violation
 */

#include <stdio.h>
 
int func(const char *filename) {
  FILE *f = fopen(filename, "r"); 
  if (NULL == f) {
    return -1;
  }
  /* ... */
  if (fclose(f) == EOF) {
    return -1;
  }
  return 0;
}