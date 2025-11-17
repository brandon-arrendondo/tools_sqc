/*
 * Rule: FIO21-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO21-C violation
 */

#include <stdio.h>
 
void func(void) {
  FILE *fp = tmpfile();
  if (fp == NULL) {
    /* Handle error */
  }
}