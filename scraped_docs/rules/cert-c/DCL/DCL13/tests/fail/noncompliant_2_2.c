void foo(const int *x) {
  if (x != NULL) {
    *x = 3; /* Compiler should generate diagnostic message */
  }
  /* ... */
}