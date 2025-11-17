/*
 * Rule: ERR33-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ERR33-C violation
 */

#include <stdio.h>
 
size_t read_at(FILE *file, long offset,
               void *buf, size_t nbytes) {
  if (fseek(file, offset, SEEK_SET) != 0) {
    /* Indicate error to caller */
    return 0;
  }
  return fread(buf, 1, nbytes, file);
}