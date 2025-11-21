/*
 * Rule: FIO15-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO15-C violation
 */

#include <stdio.h>

void unsafe_file_operation(void) {
  // VIOLATION: Creating file in /tmp without checking if it's a secure directory
  FILE *fp = fopen("/tmp/appdata/config.txt", "w");
  if (fp) {
    fprintf(fp, "sensitive data");
    fclose(fp);
  }
}