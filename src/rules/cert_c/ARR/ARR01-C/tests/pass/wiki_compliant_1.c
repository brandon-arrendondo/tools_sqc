/*
 * Rule: ARR01-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ARR01-C violation
 */

void clear(int array[], size_t len) {
  for (size_t i = 0; i < len; i++) {
    array[i] = 0;
  }
}

void dowork(void) {
  int dis[12];

  clear(dis, sizeof(dis) / sizeof(dis[0]));
  /* ... */
}