static int a;
static int b;
 
int get_sum(void) {
  return a + b;
}
 
void set_values(int new_a, int new_b) {
  a = new_a;
  b = new_b;
}