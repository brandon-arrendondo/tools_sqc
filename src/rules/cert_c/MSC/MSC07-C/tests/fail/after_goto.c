// Test: code after unconditional goto is unreachable

int compute(int x) {
    if (x < 0)
        goto error;
    return x * 2;

error:
    goto done;
    x = -1;  // MSC07-C violation
done:
    return x;
}
