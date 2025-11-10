void cleanup(void) {
  /* Delete temporary files, restore consistent state, etc. */
}

int main(void) {
  if (atexit(cleanup) != 0) {
    /* Handle error */
  }

  /* ... */

  if (/* Something bad happened */) {
    exit(EXIT_FAILURE);
  }

  /* ... */
}