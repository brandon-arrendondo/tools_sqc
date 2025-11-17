/*
 * Rule: EXP33-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP33-C violation
 */

#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
  
void func(void) {
  struct timeval tv;
  unsigned long junk;

  gettimeofday(&tv, NULL);
  srandom((getpid() << 16) ^ tv.tv_sec ^ tv.tv_usec ^ junk);
}