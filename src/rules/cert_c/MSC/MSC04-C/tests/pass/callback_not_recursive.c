// MSC04-C pass: function pointer usage is not recursion
typedef void (*callback_t)(int);

void apply(callback_t cb, int value) {
    cb(value);
}

void handler(int x) {
    (void)x;
}
