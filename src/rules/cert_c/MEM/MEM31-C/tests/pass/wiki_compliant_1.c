/*
 * Rule: MEM31-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM31-C violation
 */

#include <stdlib.h>

enum { BUFFER_SIZE = 32 };

int f(void) {
  char *text_buffer = (char *)malloc(BUFFER_SIZE); 
  if (text_buffer == NULL) {
    return -1;
  }
 
  free(text_buffer);
  return 0;
}