/*
 * Rule: PRE32-C
 * Source: wiki
 * Status: PASS - Should NOT trigger PRE32-C violation
 */

#include <string.h>

void func(const char *src) {
  /* Validate the source string; calculate size */
  char *dest;
  /* malloc() destination string */ 
  #ifdef PLATFORM1
    memcpy(dest, src, 12);
  #else
    memcpy(dest, src, 24);
  #endif
  /* ... */
}