/*
 * Rule: FIO41-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO41-C violation
 */

#include <stdio.h>
 
void func(const char *file_name) {
  int c;
  FILE *fptr;

  fptr = fopen(file_name, "r");
  if (fptr == NULL) {
    /* Handle error */
  }

  c = getc(fptr);
  if (c == EOF) {
    /* Handle error */
  }

  if (fclose(fptr) == EOF) {
    /* Handle error */
  }
}