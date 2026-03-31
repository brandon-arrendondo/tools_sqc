// MSC04-C pass: bounded recursion in void function with base case
void traverse(int *arr, int n) {
    if (n <= 0) return;
    process(arr[n - 1]);
    traverse(arr, n - 1);
}
