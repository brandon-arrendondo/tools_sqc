char *file_name;
FILE *fd;

/* Initialize file_name */

fd = fopen(file_name, "w+");
if (fd == NULL) {
  /* Handle error */
}

/*... Write to file ...*/

/* Go to beginning of file */
fseek(fd, 0, SEEK_SET);

/*... Read from file ...*/

fclose(fd);
fd = NULL;