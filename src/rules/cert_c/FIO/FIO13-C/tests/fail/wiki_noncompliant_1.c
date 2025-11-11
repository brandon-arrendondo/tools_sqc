/*
 * Rule: FIO13-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO13-C violation
 */

FILE *fp;
char *file_name;

/* Initialize file_name */

fp = fopen(file_name, "rb");
if (fp == NULL) {
  /* Handle error */
}

/* Read data */

if (ungetc('\n', fp) == EOF) {
  /* Handle error */
}
if (ungetc('\r', fp) == EOF) {
  /* Handle error */
}

/* Continue */