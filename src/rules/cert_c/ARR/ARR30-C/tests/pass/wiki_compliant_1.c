/*
 * Rule: ARR30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ARR30-C violation
 */

enum { TABLESIZE = 100 };

static int table[TABLESIZE];

int *f(int index) {
  if (index >= 0 && index < TABLESIZE) {
    return table + index;
  }
  return NULL;
}