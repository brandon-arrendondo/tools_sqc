/*
 * Rule: FIO21-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO21-C violation
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
extern int secure_dir(const char *sdn);
 
void func(void) {
  const char *sdn = "/home/usr1/";
  char sfn[] = "/home/usr1/temp-XXXXXX";
  FILE *sfp;

  if (!secure_dir(sdn)) {
    /* Handle error */
  }

  int fd = mkstemp(sfn);
  if (fd == -1) {
    /* Handle error */
  }

  /* 
   * Unlink immediately to hide the file name. The race
   * condition here is inconsequential if the file is created
   * with exclusive permissions (glibc >= 2.0.7).
   */
  if (unlink(sfn) == -1) {
    /* Handle error */
  }

  sfp = fdopen(fd, "w+");
  if (sfp == NULL) {
    close(fd);
    /* Handle error */
  }

  /* Use temporary file */

  fclose(sfp); /* Also closes fd */
}