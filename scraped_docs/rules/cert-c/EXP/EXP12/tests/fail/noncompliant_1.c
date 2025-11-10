void func(char* name) {
  char* s = NULL;
  asprintf(&s,"Hello, %s!\n", name);
  (void) puts(s);
  free(s);
}