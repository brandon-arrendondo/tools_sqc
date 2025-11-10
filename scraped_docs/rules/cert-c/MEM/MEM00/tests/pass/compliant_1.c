enum { MIN_SIZE_ALLOWED = 32 };

int verify_size(const char *list, size_t size) {
  if (size < MIN_SIZE_ALLOWED) {
    /* Handle error condition */
    return -1;
  }
  return 0;
}

void process_list(size_t number) {
  char *list = (char *)malloc(number);

  if (list == NULL) {
    /* Handle allocation error */
  }

  if (verify_size(list, number) == -1) {
      free(list);
      return;
  }

  /* Continue processing list */

  free(list);
}