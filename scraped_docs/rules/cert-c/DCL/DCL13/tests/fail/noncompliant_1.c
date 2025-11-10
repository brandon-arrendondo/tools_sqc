void foo(int *x) {
  if (x != NULL) {
    *x = 3; /* Visible outside function */
  }
  /* ... */
}