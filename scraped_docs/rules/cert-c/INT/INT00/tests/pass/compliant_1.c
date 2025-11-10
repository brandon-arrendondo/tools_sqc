int f(void) {
  FILE *fp;
  int x;
/* Initialize fp */
  if (fscanf(fp, "%d", &x) < 1) {
    return -1; /* Indicate failure */
  }

/* ... */
  return 0;
}