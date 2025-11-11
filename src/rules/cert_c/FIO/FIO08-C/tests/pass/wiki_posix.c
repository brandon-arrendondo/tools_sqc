/*
 * Rule: FIO08-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO08-C violation
 */

FILE *file;
char *file_name;

/* Initialize file_name */

file = fopen(file_name, "w+");
if (file == NULL) {
  /* Handle error condition */
}

if (unlink(file_name) != 0) {
  /* Handle error condition */
}

/* Continue performing I/O operations on file */

fclose(file);