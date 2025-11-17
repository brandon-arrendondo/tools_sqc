/*
 * Rule: FIO03-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO03-C violation
 */

char *file_name;
FILE *fp;

/* Initialize file_name */

fp = fopen(file_name, "w");
if (!fp) {
  /* Handle error */
}