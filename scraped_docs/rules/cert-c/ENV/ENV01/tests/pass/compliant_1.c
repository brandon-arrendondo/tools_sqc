void f() {
  char *path = NULL;
  /* Avoid assuming $PATH is defined or has limited length */
  const char *temp = getenv("PATH");
  if (temp != NULL) {
    path = (char*) malloc(strlen(temp) + 1);
    if (path == NULL) {
      /* Handle error condition */
    } else {
      strcpy(path, temp);
    }
    /* Use path */
    free(path);
  }
}