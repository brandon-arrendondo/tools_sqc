/*
 * Rule: INT31-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT31-C violation
 */

#include <time.h>
 
void func(void) {
  time_t now = time(NULL);
  if (now != (time_t)-1) {
    /* Continue processing */
  }
}