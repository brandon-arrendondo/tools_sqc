/*
 * Rule: MEM34-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM34-C violation
 */

#include <stdlib.h>
 
enum { BUFSIZE = 256 };
 
void f(void) {
  char *buf = (char *)malloc(BUFSIZE * sizeof(char));
  char *p = (char *)realloc(buf, 2 * BUFSIZE);
  if (p == NULL) {
    /* Handle error */
  }
}