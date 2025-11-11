/*
 * Rule: EXP03-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP03-C violation
 */

enum { buffer_size = 50 };

struct buffer {
  size_t size;
  char bufferC[buffer_size];
};

/* ... */

void func(const struct buffer *buf) {

  struct buffer *buf_cpy = 
    (struct buffer *)malloc(sizeof(struct buffer));

  if (buf_cpy == NULL) {
    /* Handle malloc() error */
  }

  /* ... */

  memcpy(buf_cpy, buf, sizeof(struct buffer));

  /* ... */

  free(buf_cpy);
}