/*
 * Rule: STR31-C
 * Source: wiki
 * Status: FAIL - Should trigger STR31-C violation
 */

#include <stdio.h>
 
#define BUFFER_SIZE 1024

void func(void) {
  char buf[BUFFER_SIZE];
  if (gets(buf) == NULL) {
    /* Handle error */
  }
}