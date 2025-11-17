/*
 * Rule: MSC33-C
 * Source: wiki
 * Status: FAIL - Should trigger MSC33-C violation
 */

#include <time.h>
 
void func(struct tm *time_tm) {
  char *time = asctime(time_tm);
  /* ... */
}