/*
 * Rule: EXP33-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP33-C violation
 */

#include <stdio.h>
enum { BUFFERSIZE = 24 }; 
void report_error(const char *msg) {
  const char *error_log;  /* NON-COMPLIANT: Uninitialized local pointer */
  char buffer[BUFFERSIZE];

  sprintf(buffer, "Error: %s", error_log);
  printf("%s\n", buffer);
}