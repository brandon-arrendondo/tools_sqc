/*
 * Rule: FIO32-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO32-C violation
 */

#include <stdio.h>
 
void func(void) {
  FILE *file;
  
  // VIOLATION: Using device file without checking file type
  if ((file = fopen("/dev/console", "r")) == NULL) {
    return;
  }

  // VIOLATION: ftell is inappropriate for device files  
  long pos = ftell(file);
  if (pos == -1L) {
    fclose(file);
    return;
  }

  fclose(file);
}