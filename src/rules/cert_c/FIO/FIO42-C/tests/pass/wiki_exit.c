/*
 * Rule: FIO42-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO42-C violation
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  FILE *f = fopen(filename, "w"); 
  if (NULL == f) {
    /* Handle error */
  }
  /* ... */
  if (fclose(f) == EOF) {
    /* Handle error */
  }
  exit(EXIT_SUCCESS);
}