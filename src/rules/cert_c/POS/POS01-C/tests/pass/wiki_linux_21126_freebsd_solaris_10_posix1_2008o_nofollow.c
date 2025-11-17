/*
 * Rule: POS01-C
 * Source: wiki
 * Status: PASS - Should NOT trigger POS01-C violation
 */

char *file_name = /* something */;
char *userbuf = /* something */;
unsigned int userlen = /* length of userbuf string */;

int fd = open(file_name, O_RDWR | O_NOFOLLOW);
if (fd == -1) {
  /* handle error */
}
write(fd, userbuf, userlen);