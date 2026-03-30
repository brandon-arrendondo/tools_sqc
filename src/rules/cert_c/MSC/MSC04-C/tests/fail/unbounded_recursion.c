// MSC04-C fail: unbounded recursion — no parameters, no base case
void infinite(void) {  // expected-warning {{MSC04-C}}
    infinite();
}
