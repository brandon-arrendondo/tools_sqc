size_t nsize;
/* Initialize nsize */
char *p2;
char *p = (char *)malloc(100);
if (p == NULL) {
  /* Handle error */
}

/* ... */

p2 = NULL;
if (nsize != 0) {
  p2 = (char *)realloc(p, nsize);
}
if (p2 == NULL) {
  free(p);
  p = NULL;
  return NULL;
}
p = p2;