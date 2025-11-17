/*
 * Rule: PRE13-C
 * Source: wiki
 * Status: PASS - Should NOT trigger PRE13-C violation
 */

#if defined(__STDC_LIB_EXT1__)
  #if (__STDC_LIB_EXT1__ >= 201112L)
    #define USE_EXT1 1
    #define __STDC_WANT_LIB_EXT1__ 1 /* Want the ext1 functions */
  #endif
#endif
 
#include <string.h>
#include <stdlib.h>

#if !defined(USE_EXT1)
  #include "safe_str_lib.h"
#endif
  
int main(void) {
  char source_msg[] = "This is a test.";
  char *msg = malloc(sizeof(source_msg) + 1);
 
  if (msg != NULL) {
    strcpy_s(msg, sizeof msg, source_msg);
  } 
  else {
    return EXIT_FAILURE;
  }
  return 0;
}