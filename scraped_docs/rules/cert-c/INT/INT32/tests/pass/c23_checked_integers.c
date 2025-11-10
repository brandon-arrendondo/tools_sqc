#include <stdckdint.h>

void func(signed int si_a, signed int si_b) {
  int product;
  if (ckd_mul(&product, si_a, si_b)) {
    /* Handle error */
  }
  /* ... */
}