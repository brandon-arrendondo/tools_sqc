/*
 * Rule: FIO44-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO44-C violation
 */

#include <stdio.h>
#include <string.h>
 
int opener(FILE *file) {
  int rc;
  fpos_t offset;

  memset(&offset, 0, sizeof(offset));

  if (file == NULL) { 
    return -1;
  }

  /* Read in data from file */

  rc = fsetpos(file, &offset);
  if (rc != 0 ) {
    return rc;
  }

  return 0;
}