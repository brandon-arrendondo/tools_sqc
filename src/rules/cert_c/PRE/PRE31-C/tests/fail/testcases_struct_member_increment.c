/*
 * Rule: PRE31-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE31-C violation
 */

/*
 * Rule: PRE31-C - Avoid side effects in arguments to unsafe macros
 * Status: FAIL
 * Reason: Struct member increment in unsafe macro
 */

#define MAX(a, b) ((a) > (b) ? (a) : (b))  /* UNSAFE */

struct data {
    int count;
    int value;
};

void process_struct(struct data *d) {
    // Member increment has side effect
    int max_val = MAX(d->count++, 10);  // Line 16 - VIOLATION
}

int main(void) {
    struct data d = {5, 20};
    process_struct(&d);
    return 0;
}
