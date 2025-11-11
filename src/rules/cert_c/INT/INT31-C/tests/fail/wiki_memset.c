/*
 * Rule: INT31-C
 * Source: wiki
 * Status: FAIL - Should trigger INT31-C violation
 */

#include <string.h>
#include <stddef.h>
 
int *init_memory(int *array, size_t n) {
  return memset(array, 4096, n); 
}