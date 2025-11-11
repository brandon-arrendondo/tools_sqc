/*
 * Rule: ERR30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ERR30-C violation
 */

#include <errno.h>
#include <stdio.h>

void func(FILE* fp) { 
  if (ftell(fp) == -1) {
    perror("ftell");
  }
}