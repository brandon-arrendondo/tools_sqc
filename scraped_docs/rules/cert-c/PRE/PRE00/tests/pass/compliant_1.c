inline int cube(int i) {
  return i * i * i;
}
 
void func(void) {
  int i = 2;
  int a = 81 / cube(++i);
  /* ... */ 
}