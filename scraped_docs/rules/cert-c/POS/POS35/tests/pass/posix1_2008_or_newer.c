char *filename = /* file name */;
char *userbuf = /* user data */;
unsigned int userlen = /* length of userbuf string */;

int fd = open(filename, O_RDWR|O_NOFOLLOW);
if (fd == -1) {
  /* Handle error */
}
if (write(fd, userbuf, userlen) < userlen) {
  /* Handle error */
}