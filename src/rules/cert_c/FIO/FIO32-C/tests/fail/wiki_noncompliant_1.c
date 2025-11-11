/*
 * Rule: FIO32-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO32-C violation
 */

#include <stdio.h>
 
void func(const char *file_name) {
  FILE *file;
  if ((file = fopen(file_name, "wb")) == NULL) {
    /* Handle error */
  }

  /* Operate on the file */

  if (fclose(file) == EOF) {
    /* Handle error */
  }
}