/*
 * Rule: DCL31-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL31-C violation
 */

#include <stddef.h>
/* No declaration for custom_alloc */

int main(void) {
  for (size_t i = 0; i < 100; ++i) {
    /* int custom_alloc() assumed - implicit function declaration */
    char *ptr = (char *)custom_alloc(0x10000000);
    *ptr = 'a';
  }
  return 0;
}
