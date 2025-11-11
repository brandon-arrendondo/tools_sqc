/*
 * Rule: MSC32-C
 * Source: wiki
 * Status: FAIL - Should trigger MSC32-C violation
 */

#include <stdio.h>
#include <stdlib.h>
 
void func(void) {
  for (unsigned int i = 0; i < 10; ++i) {
    /* Always generates the same sequence */
    printf("%ld, ", random());
  }
}