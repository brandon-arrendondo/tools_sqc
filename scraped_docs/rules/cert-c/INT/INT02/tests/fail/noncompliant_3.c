#include <limits.h>

unsigned char max = CHAR_MAX + 1;
for (char i = 0; i < max; ++i) {
  printf("i=0x%08x max=0x%08x\n", i, max);
}