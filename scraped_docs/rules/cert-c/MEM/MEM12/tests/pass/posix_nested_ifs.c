/* ... Assume the same struct used previously ... */

errno_t do_something(void) {
  FILE *fin1, *fin2;
  object_t *obj;
  errno_t ret_val = NOERR; /* Initially assume a successful return value */

  if ((fin1 = fopen("some_file", "r")) != NULL) {
    if ((fin2 = fopen("some_other_file", "r")) != NULL) {
      if ((obj = malloc(sizeof(object_t))) != NULL) {

        /* ... More code ... */

        /* Clean-up & handle errors */
        free(obj);
      } else {
        ret_val = errno;
      }
      fclose(fin2);
    } else {
      ret_val = errno;
    }
    fclose(fin1);
  } else {
    ret_val = errno;
  }

  return ret_val;
}