void func(char* name) {
  char* s = NULL;
  if (asprintf(&s,"Hello, %s!\n", name) < 0) {
    /* Handle error */
  }
  (void) puts(s);
  free(s);
}