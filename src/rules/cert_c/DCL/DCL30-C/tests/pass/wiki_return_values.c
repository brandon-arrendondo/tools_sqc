/*
 * Rule: DCL30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL30-C violation
 */

#include <stddef.h>
void init_array(char *array, size_t len) {
  /* Initialize array */
  return;
}

int main(void) {
  char array[10];
  init_array(array, sizeof(array) / sizeof(array[0]));
  /* ... */
  return 0;
}