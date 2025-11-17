/*
 * Rule: DCL17-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL17-C violation
 */

int vol_read_int(volatile int *vp) {
  return *vp;
}
volatile int *vol_id_int(volatile int *vp) {
  return vp;
}

const volatile int x;
volatile int y;
void foo(void) {
  for(*vol_id_int(&y) = 0; vol_read_int(&y) < 10; *vol_id_int(&y) = vol_read_int(&y) + 1) {
    int z = vol_read_int(&x);
  }
}