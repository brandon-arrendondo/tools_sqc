/*
 * Rule: EXP33-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP33-C violation
 */

#include <string.h> 
#include <wchar.h>
 
void func(const char *mbs) {
  size_t len;
  mbstate_t state;

  len = mbrlen(mbs, strlen(mbs), &state);
}