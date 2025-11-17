/*
 * Rule: FIO45-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO45-C violation
 */

#include <stdio.h>

void open_some_file(const char *file) {
  FILE *f = fopen(file, "wx");
  if (NULL == f) {
    /* Handle error */
  }
  /* Write to file */
  if (fclose(f) == EOF) {
    /* Handle error */
  }
}