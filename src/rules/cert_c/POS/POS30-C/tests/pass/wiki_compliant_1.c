/*
 * Rule: POS30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger POS30-C violation
 */

enum { BUFFERSIZE = 1024 };
char buf[BUFFERSIZE];
ssize_t len = readlink("/usr/bin/perl", buf, sizeof(buf)-1);

if (len != -1) {
  buf[len] = '\0';
}
else {
  /* handle error condition */
}