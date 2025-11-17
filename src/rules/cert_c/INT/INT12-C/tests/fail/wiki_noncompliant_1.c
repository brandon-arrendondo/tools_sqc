/*
 * Rule: INT12-C
 * Source: wiki
 * Status: FAIL - Should trigger INT12-C violation
 */

struct {
  int a: 8;
} bits = {255};

int main(void) {
  printf("bits.a = %d.\n", bits.a);
  return 0;
}