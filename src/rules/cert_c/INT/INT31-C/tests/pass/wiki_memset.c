/*
 * Rule: INT31-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT31-C violation
 */

#include <string.h>
#include <stddef.h>

int *init_memory(int *array, size_t n) {
  return memset(array, 0, n); 
}