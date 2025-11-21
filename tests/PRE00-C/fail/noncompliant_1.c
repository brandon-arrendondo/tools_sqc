// PRE00-C: Noncompliant - function-like macro without type safety
#define SQUARE(x) ((x) * (x))  // VIOLATION: Macro has no type checking

void test_pre00c_fail() {
    int result = SQUARE(5);  // Expands to ((5) * (5))
}
