/*
 * Rule: ARR30-C
 * Source: wiki
 * Status: FAIL - Should trigger ARR30-C violation
 */

enum { TABLESIZE = 100 };

static int table[TABLESIZE];

int *f(int index) {
  if (index < TABLESIZE) {
    return table + index;
  }
  return NULL;
}