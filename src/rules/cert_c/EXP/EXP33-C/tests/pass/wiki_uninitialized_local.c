/*
 * Rule: EXP33-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP33-C violation
 */

#include <stdio.h>
 
void report_error(const char *msg) {
  printf("Error: %s\n", msg);
}