long num_long;
errno = 0;

if (scanf("%ld", &num_long) != 1) {
  /* Handle error */
}
else if (ERANGE == errno) {
  if (puts("number out of range\n") == EOF) {
      /* Handle error */
  }
}