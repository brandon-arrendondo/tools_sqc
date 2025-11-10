#include <stdio.h>

#if __STDC_VERSION__ < 201112L
#error This code requires a compiler supporting the C11 standard or newer
#endif

struct X { char a[8]; };

struct X salutation(void) {
  struct X result = { "Hello" };
  return result;
}

struct X addressee(void) {
  struct X result = { "world" };
  return result;
}

int main(void) {
  printf("%s, %s!\n", salutation().a, addressee().a);
  return 0;
}