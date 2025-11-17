/*
 * Rule: ARR01-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ARR01-C violation
 */

enum {ARR_LEN = 100};

void clear(int a[], size_t len) {
  memset(a, 0, len * sizeof(int));
}

int main(void) {
  int b[ARR_LEN];
  clear(b, ARR_LEN);
  assert(b[ARR_LEN / 2]==0); /* Cannot fail */
  return 0;
}