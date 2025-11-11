/*
 * Rule: FIO42-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO42-C violation
 */

#include <stdio.h>
 
int func(const char *filename) {
  FILE *f = fopen(filename, "r"); 
  if (NULL == f) {
    return -1;
  }
  /* ... */
  return 0;
}