/*
 * Rule: MEM30-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM30-C violation
 */

#include <stdlib.h>
 
void f(char *c_str1, size_t size) {
  char *c_str2 = (char *)realloc(c_str1, size);
  if (c_str2 == NULL) {
    free(c_str1);
  }
}