// Test: code after loop-with-continue is reachable

void process(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        if (arr[i] < 0)
            continue;
        arr[i] *= 2;  // reachable for non-negative elements
    }
}
