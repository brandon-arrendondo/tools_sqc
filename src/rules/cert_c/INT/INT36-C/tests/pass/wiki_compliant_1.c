/*
 * Rule: INT36-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT36-C violation
 */

#include <stdint.h>
 
void f(void) {
  char *ptr;
  /* ... */
  uintptr_t number = (uintptr_t)ptr;  
  /* ... */
}