/*
 * Rule: ARR01-C
 * Source: wiki
 * Status: FAIL - Should trigger ARR01-C violation
 */

void clear(int array[]) {
  for (size_t i = 0; i < sizeof(array) / sizeof(array[0]); ++i) {
     array[i] = 0;
   }
}

void dowork(void) {
  int dis[12];

  clear(dis);
  /* ... */
}