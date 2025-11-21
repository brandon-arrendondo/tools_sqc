// MEM07-C: Compliant - overflow check before calloc
// Source: https://wiki.sei.cmu.edu/confluence/display/c/MEM07-C

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

void test_mem07c_compliant() {
  long *buffer;
  size_t num_elements = 1000000;

  // OK: Check for overflow before calloc
  if (num_elements > SIZE_MAX/sizeof(long)) {
    /* Handle error condition */
    return;
  }
  buffer = (long *)calloc(num_elements, sizeof(long));
  if (buffer == NULL) {
    /* Handle error condition */
  }
  free(buffer);
}
