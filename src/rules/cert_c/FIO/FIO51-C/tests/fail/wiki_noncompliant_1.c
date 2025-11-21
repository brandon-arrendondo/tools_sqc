/*
 * Rule: FIO51-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO51-C violation
 */

#include <stdio.h>
#include <stdlib.h>

void f(const char *fileName) {
  FILE *file = fopen(fileName, "r");
  if (file == NULL) {
    // Handle error
    return;
  }
  // Use file...
  
  // VIOLATION: Terminate without closing file
  exit(1);
}