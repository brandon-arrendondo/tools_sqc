/*
 * Rule: DCL31-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL31-C violation
 */

#include <stdlib.h>
 
int main(void) {
  for (size_t i = 0; i < 100; ++i) {
    char *ptr = (char *)malloc(0x10000000);
    *ptr = 'a';
  }
  return 0;
}