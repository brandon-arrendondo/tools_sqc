/*
 * Rule: EXP03-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP03-C violation
 */

enum { buffer_size = 50 };

struct buffer {
  size_t size;
  char bufferC[buffer_size];
};

/* ... */

void func(const struct buffer *buf) {

  /*
   * Incorrectly assumes sizeof(struct buffer) =
   * sizeof(size_t) + sizeof(bufferC)
   */
  struct buffer *buf_cpy = (struct buffer *)malloc(
    sizeof(size_t) + (buffer_size * sizeof(char) /* 1 */)
  );

  if (buf_cpy == NULL) {
    /* Handle malloc() error */
  }

  /* 
   * With padding, sizeof(struct buffer) may be greater than
   * sizeof(size_t) + sizeof(buff.bufferC), causing some data  
   * to be written outside the bounds of the memory allocated.
   */
  memcpy(buf_cpy, buf, sizeof(struct buffer));

  /* ... */

  free(buf_cpy);
}