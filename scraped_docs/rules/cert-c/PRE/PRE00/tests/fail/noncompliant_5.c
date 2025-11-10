int operations = 0, calls_to_F = 0, calls_to_G = 0;
 
#define F(x) (++operations, ++calls_to_F, 2 * x)
#define G(x) (++operations, ++calls_to_G, x + 1)

void func(int x) {
  int y = F(x) + G(x);

}