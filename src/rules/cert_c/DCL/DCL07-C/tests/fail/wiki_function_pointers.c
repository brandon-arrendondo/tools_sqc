/*
 * Rule: DCL07-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL07-C violation
 */

int add(int x, int y, int z) {
  return x + y + z;
}

int main(int argc, char *argv[]) {
  int (*fn_ptr) (int, int);
  int res;
  fn_ptr = add;
  res = fn_ptr(2, 3);  /* Incorrect */
  /* ... */
  return 0;
}