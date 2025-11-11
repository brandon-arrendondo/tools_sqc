/*
 * Rule: EXP39-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP39-C violation
 */

#include <stdio.h>
 
void f(void) {
  if (sizeof(int) == sizeof(float)) {
    float f = 0.0f;
    int *ip = (int *)&f;
    (*ip)++;
    printf("float is %f\n", f);
  }
}