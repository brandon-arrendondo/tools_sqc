/*
 * Rule: PRE32-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE32-C violation
 */

#include <string.h>
 
void func(const char *src) {
  /* Validate the source string; calculate size */
  char *dest;
  /* malloc() destination string */ 
  memcpy(dest, src,
    #ifdef PLATFORM1
      12
    #else
      24
    #endif
  );
  /* ... */
}