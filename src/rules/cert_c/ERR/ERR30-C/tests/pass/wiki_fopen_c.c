/*
 * Rule: ERR30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ERR30-C violation
 */

#include <stdio.h>
 
void func(const char *filename) {
  FILE *fileptr = fopen(filename, "rb");
  if (fileptr == NULL)  {
    /* An error occurred in fopen() */
  }
}