/*
 * Rule: MEM04-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM04-C violation
 */

size_t nsize = /* Some value, possibly user supplied */;
char *p2;
char *p = (char *)malloc(100);
if (p == NULL) {
  /* Handle error */
}

/* ... */

if ((p2 = (char *)realloc(p, nsize)) == NULL) {
  free(p);
  p = NULL;
  return NULL;
}
p = p2;