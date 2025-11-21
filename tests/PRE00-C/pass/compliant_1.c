// PRE00-C: Compliant - inline function with type safety
static inline int square(int x) {  // OK: Type-safe inline function
    return x * x;
}

void test_pre00c_pass() {
    int result = square(5);
}
