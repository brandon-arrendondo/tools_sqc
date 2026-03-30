// Test: code after if-with-return is reachable (return is in nested scope)

int abs_val(int x) {
    if (x < 0) {
        return -x;
    } else {
        return x;
    }
}

int clamp(int x, int lo, int hi) {
    if (x < lo)
        return lo;
    if (x > hi)
        return hi;
    return x;
}
