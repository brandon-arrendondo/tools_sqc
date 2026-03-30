// Test: code after unconditional continue is unreachable

void process(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        if (arr[i] < 0) {
            continue;
            arr[i] = 0;  // MSC07-C violation
        }
    }
}
