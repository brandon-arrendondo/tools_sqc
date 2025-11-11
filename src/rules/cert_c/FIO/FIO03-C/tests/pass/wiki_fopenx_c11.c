/*
 * Rule: FIO03-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO03-C violation
 */

char *file_name;
FILE *fp;

/* Initialize file_name */

fp = fopen(file_name, "wx");
if (!fp) {
  /* Handle error */
}