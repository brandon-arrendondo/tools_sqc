/*
 * Rule: ARR01-C
 * Source: wiki
 * Status: FAIL - Should trigger ARR01-C violation
 */

enum {ARR_LEN = 100};

void clear(int a[ARR_LEN]) {
  memset(a, 0, sizeof(a)); /* Error */
}

int main(void) {
  int b[ARR_LEN];
  clear(b);
  assert(b[ARR_LEN / 2]==0); /* May fail */
  return 0;
}