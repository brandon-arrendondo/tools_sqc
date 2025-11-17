/*
 * Rule: MEM05-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM05-C violation
 */

int copy_file(FILE *src, FILE *dst, size_t bufsize) {
  char buf[bufsize];

  while (fgets(buf, bufsize, src)) {
    if (fputs(buf, dst) == EOF) {
      /* Handle error */
    }
  }

  return 0;
}