void f() {
  char path[PATH_MAX]; /* Requires PATH_MAX to be defined */
  strcpy(path, getenv("PATH"));
  /* Use path */
}