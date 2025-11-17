/*
 * Rule: API07-C
 * Source: wiki
 * Status: FAIL - Should trigger API07-C violation
 */

char *source;
char a[NTBS_SIZE];
/* ... */
if (source) {
  char* b = strncpy(a, source, 5); // b == a
}
else {
  /* Handle null string condition */
}