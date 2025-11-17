/*
 * Rule: FIO40-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO40-C violation
 */

#include <stdio.h>
 
enum { BUFFER_SIZE = 1024 };
void func(FILE *file) {
  char buf[BUFFER_SIZE];

  if (fgets(buf, sizeof(buf), file) == NULL) {
    /* Set error flag and continue */
  }
}