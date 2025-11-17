/*
 * Rule: INT12-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT12-C violation
 */

struct {
  unsigned int a: 8;
} bits = {255};

int main(void) {
  printf("bits.a = %d.\n", bits.a);
  return 0;
}