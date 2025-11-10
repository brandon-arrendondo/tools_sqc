#include <limits.h>

unsigned int pow2(unsigned int exp) {
  if (exp >= UINT_WIDTH) {
    /* Handle error */
  }
  return 1 << exp;
}