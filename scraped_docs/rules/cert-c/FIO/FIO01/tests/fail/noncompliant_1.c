char *file_name;
FILE *f_ptr;

/* Initialize file_name */

f_ptr = fopen(file_name, "w");
if (f_ptr == NULL) {
  /* Handle error */
}

/*... Process file ...*/

if (fclose(f_ptr) != 0) {
  /* Handle error */
}

if (remove(file_name) != 0) {
  /* Handle error */
}