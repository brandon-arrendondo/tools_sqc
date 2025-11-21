/*
 * Rule: FIO32-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO32-C violation
 */

#include <stdio.h>

void func(void) {
  FILE *file;
  // VIOLATION: Using fseek on a device file
  if ((file = fopen("/dev/tty", "wb")) == NULL) {
    return;
  }

  // VIOLATION: fseek is inappropriate for device files
  fseek(file, 0, SEEK_SET);

  if (fclose(file) == EOF) {
    return;
  }
}