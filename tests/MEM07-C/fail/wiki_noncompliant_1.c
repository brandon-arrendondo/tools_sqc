// MEM07-C: Noncompliant - no overflow check before calloc
// Source: https://wiki.sei.cmu.edu/confluence/display/c/MEM07-C

#include <stdlib.h>
#include <stddef.h>

void test_mem07c() {
  size_t num_elements = 1000000;
  
  // VIOLATION: No check for overflow before calling calloc
  long *buffer = (long *)calloc(num_elements, sizeof(long));
  if (buffer == NULL) {
    /* Handle error condition */
  }
  /* ... */
  free(buffer);
  buffer = NULL;
}
