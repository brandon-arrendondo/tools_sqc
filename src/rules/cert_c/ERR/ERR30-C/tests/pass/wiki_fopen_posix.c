/*
 * Rule: ERR30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ERR30-C violation
 */

#include <errno.h>
#include <stdio.h>
 
void func(const char *filename) {
  FILE *fileptr;

  errno = 0;
  fileptr = fopen(filename, "rb");
  if (fileptr == NULL)  {
    /*
     * An error occurred in fopen(); now it's valid 
     * to examine errno.
     */
    perror(filename);
  }
}