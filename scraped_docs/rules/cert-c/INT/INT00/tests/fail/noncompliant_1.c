int f(void) {
  FILE *fp;
  int x;
/* ... */
  if (fscanf(fp, "%ld", &x) < 1) {
    return -1; /* Indicate failure */
  }

/* ... */
  return 0;
}