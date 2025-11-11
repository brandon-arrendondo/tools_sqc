/*
 * Rule: POS30-C
 * Source: wiki
 * Status: FAIL - Should trigger POS30-C violation
 */

long symlink_max;
size_t bufsize;
char *buf;
ssize_t len;

errno = 0;
symlink_max = pathconf("/usr/bin/", _PC_SYMLINK_MAX);
if (symlink_max == -1) {
  if (errno != 0) {
    /* handle error condition */
  }
  bufsize = 10000;
}
else {
  bufsize = symlink_max+1;
}

buf = (char *)malloc(bufsize);
if (buf == NULL) {
  /* handle error condition */
}

len = readlink("/usr/bin/perl", buf, bufsize);
buf[len] = '\0';