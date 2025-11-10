char *source;
char a[NTBS_SIZE];
/* ... */
if (source) {
  errno_t err = strncpy_s(a, sizeof(a), source, 5);
  if (err != 0) {
    /* Handle error */
  }
}
else {
  /* Handle null string condition */
}