/*
 * Rule: ENV30-C
 * Source: wiki
 * Status: FAIL - Should trigger ENV30-C violation
 */

#include <locale.h>
 
void f2(void) {
  struct lconv *conv = localeconv();
 
  if ('\0' == conv->decimal_point[0]) {
    conv->decimal_point = ".";
  }
}