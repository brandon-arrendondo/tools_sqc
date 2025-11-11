/*
 * Rule: ERR30-C
 * Source: wiki
 * Status: FAIL - Should trigger ERR30-C violation
 */

#include <errno.h>
#include <stdio.h>

void func(FILE* fp) { 
  errno=0;
  ftell(fp);
  if (errno) {
    perror("ftell");
  }
}