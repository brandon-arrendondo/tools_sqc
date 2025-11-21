/*
 * Rule: FIO41-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO41-C violation
 */

#include <stdio.h>

FILE *get_stream(void) {
  return stdin;
}

void func(void) {
  // VIOLATION: getc() with stream argument that has side effects
  int c = getc(get_stream());
  if (c == EOF) {
    return;
  }
}