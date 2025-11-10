char *file_name;
FILE *fd;

/* Initialize file_name */

fd = fopen(file_name, "r+");
if (fd == NULL) {
  /* Handle error */
}

/* Read user's file */

fclose(fd);
fd = NULL;