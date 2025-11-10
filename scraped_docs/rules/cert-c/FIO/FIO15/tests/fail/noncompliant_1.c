char *file_name;
FILE *fp;

/* Initialize file_name */

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