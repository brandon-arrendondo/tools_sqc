/*
 * Rule: FIO34-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO34-C violation
 */

#include <stdio.h>
 
void func(void) {
  int c;
 
  do {
    c = getchar();
  } while (c != EOF);
}