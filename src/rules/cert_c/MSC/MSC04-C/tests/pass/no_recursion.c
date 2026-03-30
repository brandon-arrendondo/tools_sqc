// MSC04-C pass: no recursion — each function calls a different one
int helper(int x) {
    return x * 2;
}

int compute(int n) {
    return helper(n) + 1;
}
