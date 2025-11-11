/*
 * Rule: FIO03-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO03-C violation
 */

char *file_name;
int new_file_mode;
FILE *fp;
int fd;

/* Initialize file_name and new_file_mode */

fd = open(file_name, O_CREAT | O_EXCL | O_WRONLY, new_file_mode);
if (fd == -1) {
  /* Handle error */
}

fp = fdopen(fd, "w");
if (fp == NULL) {
  /* Handle error */
}