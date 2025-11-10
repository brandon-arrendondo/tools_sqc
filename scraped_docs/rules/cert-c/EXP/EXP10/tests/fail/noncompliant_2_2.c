#include <stdio.h>

int g;

int f(int i) {
  g = i;
  return i;
}

int main(void) {
  int x = f(1) + f(2);
  printf("g = %d\n", g);
  /* ... */
  return 0;
}