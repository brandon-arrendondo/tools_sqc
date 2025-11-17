/*
 * Rule: ERR06-C
 * Source: wiki
 * Status: FAIL - Should trigger ERR06-C violation
 */

void cleanup(void) {
  /* Delete temporary files, restore consistent state, etc. */
}

int main(void) {
  if (atexit(cleanup) != 0) {
    /* Handle error */
  }

  /* ... */

  assert(/* Something bad didn't happen */);

  /* ... */
}