/*
 * Rule: FIO38-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO38-C violation
 */

#include <stdio.h>
#include <string.h>

int main(void) {
  FILE my_stdout;

  // VIOLATION: Copying FILE object using memcpy
  memcpy(&my_stdout, stdout, sizeof(FILE));

  if (fputs("Hello, World!\n", &my_stdout) == EOF) {
    return 1;
  }
  return 0;
}