/*
 * Rule: FIO30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO30-C violation
 */

#include <stdio.h>
 
void incorrect_password(const char *user) {
  static const char msg_format[] = "%s cannot be authenticated.\n";
  fprintf(stderr, msg_format, user);
}