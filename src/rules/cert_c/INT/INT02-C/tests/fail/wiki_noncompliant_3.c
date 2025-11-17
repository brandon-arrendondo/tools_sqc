/*
 * Rule: INT02-C
 * Source: wiki
 * Status: FAIL - Should trigger INT02-C violation
 */

#include <limits.h>

unsigned char max = CHAR_MAX + 1;
for (char i = 0; i < max; ++i) {
  printf("i=0x%08x max=0x%08x\n", i, max);
}