enum { MAX = 100 };

int helper(int i) {
  /* Perform some computation based on i */
}

int main(void) {
  size_t i;
  int out[MAX];

  for (i = 0; i < MAX; i++) {
    out[i] = helper(i);
  }

  /* ... */

}