#include <stdio.h>

struct X { int a[6]; };

struct X addressee(void) {
  struct X result = { { 1, 2, 3, 4, 5, 6 } };
  return result;
}

int main(void) {
  struct X my_x = addressee();
  int *my_a = my_x.a;
  printf("%x", my_a[0]);
  return 0;
}