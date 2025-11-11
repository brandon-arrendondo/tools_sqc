/*
 * Rule: EXP33-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP33-C violation
 */

#include <string.h> 
#include <wchar.h>
 
void func(const char *mbs) {
  size_t len;
  mbstate_t state;

  memset(&state, 0, sizeof(state));
  len = mbrlen(mbs, strlen(mbs), &state);
}