/*
 * Rule: ENV33-C
 * Source: wiki
 * Status: FAIL - Should trigger ENV33-C violation
 */

#include <stdlib.h>
 
void func(void) {
  system("rm ~/.config");
}