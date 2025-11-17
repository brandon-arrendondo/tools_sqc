/*
 * Rule: ERR34-C
 * Source: wiki
 * Status: FAIL - Should trigger ERR34-C violation
 */

#include <stdlib.h>
 
void func(const char *buff) {
  int si;

  if (buff) {
    si = atoi(buff);
  } else {
    /* Handle error */
  }
}