/*
 * Rule: FIO34-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO34-C violation
 */

#include <assert.h>
#include <limits.h>
#include <stdio.h>

void func(void) {
  char c;
  static_assert(UCHAR_MAX < UINT_MAX, "FIO34-C violation");

  do {
    c = getchar();
  } while (c != EOF);
}