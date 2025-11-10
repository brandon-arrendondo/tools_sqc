#define ABS(x) (((x) < 0) ? -(x) : (x))

void f(int n) {
  int m;
  m = ABS(++n);
  /* ... */
}