// MSC04-C pass: bounded recursion with parameter-dependent base case
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
