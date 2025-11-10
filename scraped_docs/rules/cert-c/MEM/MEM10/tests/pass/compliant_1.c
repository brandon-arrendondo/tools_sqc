void incr(int *intptr) {
  if (!valid(intptr)) {
    /* Handle error */
  }
  (*intptr)++;
}