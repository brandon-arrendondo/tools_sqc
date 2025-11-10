int add(int x, int y, int z) {
  return x + y + z;
}

int main(int argc, char *argv[]) {
  int (*fn_ptr) (int, int, int) ;
  int res;
  fn_ptr = add;
  res = fn_ptr(2, 3, 4);
  /* ... */
  return 0;
}