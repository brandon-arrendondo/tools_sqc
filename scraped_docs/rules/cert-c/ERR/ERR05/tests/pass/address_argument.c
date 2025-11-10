const errno_t ESOMETHINGREALLYBAD = 1;

void g(errno_t * err) {
  if (err == NULL) {
    /* Handle null pointer */
  }
  /* ... */
  if (something_really_bad_happens) {
    *err = ESOMETHINGREALLYBAD;
  } else {
    /* ... */
    *err = 0;
  }
}

void f(errno_t * err) {
  if (err == NULL) {
    /* Handle null pointer */
  }
  g(err);
  if (*err == 0) {
    /* ... Do the rest of f ... */
  }
  return 0;
}