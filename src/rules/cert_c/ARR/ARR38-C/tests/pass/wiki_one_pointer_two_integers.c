/*
 * Rule: ARR38-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ARR38-C violation
 */

#include <stddef.h>
#include <stdio.h>
 
void f(FILE *file) {
  enum { BUFFER_SIZE = 1024 };
  wchar_t wbuf[BUFFER_SIZE];

  const size_t size = sizeof(*wbuf);
  const size_t nitems = sizeof(wbuf) / size;

  size_t nread = fread(wbuf, size, nitems, file);
  /* ... */
}