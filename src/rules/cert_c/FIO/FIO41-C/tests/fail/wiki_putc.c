/*
 * Rule: FIO41-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO41-C violation
 */

#include <stdio.h>

FILE *get_output_stream(void) {
  return stdout;
}
 
void func(void) {
  int c = 'a';
 
  while (c <= 'z') {
    // VIOLATION: putc() with stream argument that has side effects
    if (putc(c++, get_output_stream()) == EOF) {
      return;
    }
  }
}