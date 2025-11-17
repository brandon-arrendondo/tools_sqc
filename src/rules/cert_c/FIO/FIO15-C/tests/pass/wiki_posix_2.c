/*
 * Rule: FIO15-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO15-C violation
 */

char *dir_name;
const char *file_name = "passwd"; /* File name within the secure directory */
FILE *fp;

/* Initialize dir_name */

if (!secure_dir(dir_name)) {
  /* Handle error */
}

if (chdir(dir_name) == -1) {
  /* Handle error */
}

fp = fopen(file_name, "w");
if (fp == NULL) {
  /* Handle error */
}

/* ... Process file ... */

if (fclose(fp) != 0) {
  /* Handle error */
}

if (remove(file_name) != 0) {
  /* Handle error */
}