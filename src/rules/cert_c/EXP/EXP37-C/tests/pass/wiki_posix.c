/*
 * Rule: EXP37-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP37-C violation
 */

#include <fcntl.h>
 
void func(const char *ms, mode_t perms) {
  /* ... */
  int fd;
  fd = open(ms, O_CREAT | O_EXCL | O_WRONLY | O_TRUNC, perms);
  if (fd == -1) {
    /* Handle error */
  }
}