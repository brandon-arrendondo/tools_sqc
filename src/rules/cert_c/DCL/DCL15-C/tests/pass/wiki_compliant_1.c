/*
 * Rule: DCL15-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL15-C violation
 */

enum {MAX = 100};

static int helper(int i) {
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