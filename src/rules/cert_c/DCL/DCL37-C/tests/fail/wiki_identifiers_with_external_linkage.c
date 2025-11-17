/*
 * Rule: DCL37-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL37-C violation
 */

#include <stddef.h>
 
void *malloc(size_t nbytes) {
  void *ptr;
  /* Allocate storage from own pool and set ptr */
  return ptr;
}

void free(void *ptr) {
  /* Return storage to own pool */
}