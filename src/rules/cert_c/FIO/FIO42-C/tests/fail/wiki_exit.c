/*
 * Rule: FIO42-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO42-C violation
 */

#include <stdio.h>
#include <stdlib.h>
  
int main(void) {
  FILE *f = fopen(filename, "w"); 
  if (NULL == f) {
    exit(EXIT_FAILURE);
  }
  /* ... */
  exit(EXIT_SUCCESS);
}