size_t count = 0;

void g(void) {
  printf("Called g, count = %zu.\n", count);
}

typedef void (*exec_func)(void);
inline void exec_bump(exec_func f) {
  f();
  ++count;
}

void aFunc(void) {
  size_t count = 0;
  while (count++ < 10) {
    exec_bump(g);
  }
}