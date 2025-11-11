/*
 * Rule: MEM05-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM05-C violation
 */

int copy_file(FILE *src, FILE *dst, size_t bufsize) {
  if (bufsize == 0) {
    /* Handle error */
  }
  char *buf = (char *)malloc(bufsize);
  if (!buf) {
    /* Handle error */
  }

  while (fgets(buf, bufsize, src)) {
    if (fputs(buf, dst) == EOF) {
      /* Handle error */
    }
  }
  /* ... */
  free(buf);
  return 0;
}