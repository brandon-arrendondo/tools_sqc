/*
 * Rule: MEM05-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM05-C violation
 */

unsigned long fib2(unsigned int n) {
  if (n == 0) {
    return 0;
  }
  else if (n == 1 || n == 2) {
    return 1;
  }

  unsigned long prev = 1;
  unsigned long cur = 1;

  unsigned int i;

  for (i = 3; i <= n; i++) {
    unsigned long tmp = cur;
    cur = cur + prev;
    prev = tmp;
  }

  return cur;
}