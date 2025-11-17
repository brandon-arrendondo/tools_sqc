/*
 * Rule: FIO24-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO24-C violation
 */

#include <stdio.h>
 
void do_stuff(FILE *logfile) {
  /* Write logs pertaining to do_stuff() */
  fprintf(logfile, "do_stuff\n");
}

int main(void) {
  FILE *logfile = fopen("log", "a");
  if (logfile == NULL) {
    /* Handle error */
  }

  /* Write logs pertaining to main() */
  fprintf(logfile, "main\n");

  do_stuff(logfile);
 
  if (fclose(logfile) == EOF) {
    /* Handle error */
  }
  return 0;
}