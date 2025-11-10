char *file_name;
int fd;

/* Initialize file_name */

fd = open(file_name, O_CREAT | O_WRONLY);
/* Access permissions were missing */

if (fd == -1){
  /* Handle error */
}