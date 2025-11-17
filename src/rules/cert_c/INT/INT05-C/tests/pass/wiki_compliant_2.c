/*
 * Rule: INT05-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT05-C violation
 */

char buff[25];
char *end_ptr;
long num_long;

if (fgets(buff, sizeof(buff), stdin) == NULL) {
  if (puts("EOF or read error\n") == EOF) {
    /* Handle error */
  }
} else {
  errno = 0;

  num_long = strtol(buff, &end_ptr, 10);

  if (ERANGE == errno) {
    if (puts("number out of range\n") == EOF) {
      /* Handle error */
    }
  }
  else if (end_ptr == buff) {
    if (puts("not valid numeric input\n") == EOF) {
      /* Handle error */
    }
  }
  else if ('\n' != *end_ptr && '\0' != *end_ptr) {
    if (puts("extra characters on input line\n") == EOF) {
      /* Handle error */
    }
  }
}