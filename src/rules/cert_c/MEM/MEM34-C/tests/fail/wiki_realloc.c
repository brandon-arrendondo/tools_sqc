/*
 * Rule: MEM34-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM34-C violation
 */

#include <stdlib.h>
 
enum { BUFSIZE = 256 };
 
void f(void) {
  char buf[BUFSIZE];
  char *p = (char *)realloc(buf, 2 * BUFSIZE);
  if (p == NULL) {
    /* Handle error */
  }
}