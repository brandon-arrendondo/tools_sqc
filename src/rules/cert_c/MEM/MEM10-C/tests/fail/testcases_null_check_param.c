/*
 * Rule: MEM10-C
 * Source: testcases
 * Status: FAIL - Direct NULL checks on pointer parameters
 */

/* Direct NULL check with == NULL */
void process_data(int *data) {
    if (data == NULL) {
        return;
    }
    *data = 42;
}

/* Direct NULL check with !ptr */
void read_value(int *ptr) {
    if (!ptr) {
        return;
    }
    int val = *ptr;
    (void)val;
}

/* Multiple pointer params, each checked directly */
void copy_value(int *src, int *dest) {
    if (src == NULL) {
        return;
    }
    if (dest == NULL) {
        return;
    }
    *dest = *src;
}

/* NULL comparison in ternary */
int get_or_default(int *ptr, int def) {
    if (ptr == NULL) {
        return def;
    }
    return *ptr;
}
