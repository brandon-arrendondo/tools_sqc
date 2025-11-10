char *file_name;
FILE *fp;

/* Initialize file_name */

fp = fopen(file_name, "wx");
if (!fp) {
  /* Handle error */
}