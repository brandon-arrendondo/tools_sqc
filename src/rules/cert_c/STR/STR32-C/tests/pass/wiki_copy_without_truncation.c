/*
 * Rule: STR32-C
 * Source: wiki
 * Status: PASS - Should NOT trigger STR32-C violation
 */

#include <string.h>
 
enum { STR_SIZE = 32 };
 
size_t func(const char *source) {
  char c_str[STR_SIZE];
  size_t ret = 0;

  if (source) {
    if (strnlen(source, sizeof(c_str)) < sizeof(c_str)) {
      strcpy(c_str, source);
      ret = strlen(c_str);
    } else {
      /* Handle string-too-large */
    }
  } else {
    /* Handle null pointer */
  }
  return ret;
}