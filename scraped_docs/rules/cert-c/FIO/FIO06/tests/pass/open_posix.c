char *file_name;
int file_access_permissions;

/* Initialize file_name and file_access_permissions */

int fd = open(
  file_name,
  O_CREAT | O_WRONLY,
  file_access_permissions
);
if (fd == -1){
  /* Handle error */
}