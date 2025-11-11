/*
 * Rule: INT01-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT01-C violation
 */

void *alloc(rsize_t blocksize) {
  if (blocksize == 0 || blocksize > RSIZE_MAX) {
    return NULL;  /* Indicate failure */
  }
  return malloc(blocksize);
}

int read_counted_string(int fd) {
  rsize_t length;
  unsigned char *data;

  if (read_integer_from_network(fd, &length)) {
    return -1;
  }

  data = (unsigned char*)alloc(length+1);
  if (data == NULL) {
    return -1; /* Indicate failure */
  }

  if (read_network_data(fd, data, length)) {
    free(data);
    return -1;
  }
  data[length] = '\0';

  /* ... */
  free( data);
  return 0;
}