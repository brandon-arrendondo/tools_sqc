/*
 * Rule: FIO08-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO08-C violation
 */

char *file_name;
FILE *file;

/* Initialize file_name */

file = fopen(file_name, "w+");
if (file == NULL) {
  /* Handle error condition */
}

/* ... */

if (remove(file_name) != 0) {
  /* Handle error condition */
}

/* Continue performing I/O operations on file */

fclose(file);