/*
 * Rule: FIO15-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO15-C violation
 */

#include <stdio.h>
#include <stdlib.h>

void process_file(void) {
  FILE *fp;

  // VIOLATION: Using /tmp directly without security checks
  fp = fopen("/tmp/myfile", "w");
  if (fp == NULL) {
    return;
  }

  fprintf(fp, "data");

  if (fclose(fp) != 0) {
    return;
  }

  if (remove("/tmp/myfile") != 0) {
    return;
  }
}