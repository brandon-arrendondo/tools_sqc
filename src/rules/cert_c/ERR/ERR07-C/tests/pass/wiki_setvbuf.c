/*
 * Rule: ERR07-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ERR07-C violation
 */

FILE *file;
char *buf = NULL;
/* Setup file */
if (setvbuf(file, buf, buf ? _IOFBF : _IONBF, BUFSIZ) != 0) {
  /* Handle error */
}
/* ... */