/*
 * Rule: FIO45-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO45-C violation
 */

#include <stdio.h>

void open_some_file(const char *file) {
  FILE *f = fopen(file, "r");
  if (NULL != f) {
    /* File exists, handle error */
  } else {
    f = fopen(file, "w");
    if (NULL == f) {
      /* Handle error */
    }
 
    /* Write to file */
    if (fclose(f) == EOF) {
      /* Handle error */
    }
  }
}